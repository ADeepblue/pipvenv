/*
 * pipvenv.c
 * 编译: gcc pipvenv.c -o pipvenv.exe -static
 * 用法:
 *   pipvenv.exe install  pkg1 [pkg2 ...]
 *   pipvenv.exe run      envName [cmd ...]
 *   pipvenv.exe uninstall envName
 *   pipvenv.exe list
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <windows.h>
#define DIRSEP "\\"
#define snprintf _snprintf
#else
#define DIRSEP "/"
#endif

/* ---------- 工具函数 ---------- */
static void die(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}
static int exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}
static void get_exe_dir(char *buf, size_t len) {
#ifdef _WIN32
    GetModuleFileNameA(NULL, buf, (DWORD)len);
    char *p = strrchr(buf, '\\');
    if (p) *(p + 1) = '\0';
#else
    strncpy(buf, "./", len);
#endif
}
static void strip_quotes(char *s) {
    size_t l = strlen(s);
    if (l >= 2 && s[0] == '"' && s[l - 1] == '"') {
        memmove(s, s + 1, l - 2);
        s[l - 2] = '\0';
    }
}

/* ---------- ini 操作 ---------- */
static void write_ini(const char *path, const char *value) {
    FILE *fp = fopen(path, "w");
    if (!fp) die("无法写入 pipvenv.ini");
    fprintf(fp, "MAIN_PATH=\"%s\"\n", value);
    fclose(fp);
}
static int read_ini(const char *ini_path, char *out, size_t out_len) {
    if (!exists(ini_path)) {
        printf("未找到 %s，请输入虚拟环境根目录（留空退出）:\n> ", ini_path);
        fflush(stdout);
        if (!fgets(out, out_len, stdin)) return 0;
        out[strcspn(out, "\r\n")] = 0;
        strip_quotes(out);
        if (out[0] == '\0') return 0;
        if (!exists(out)) {
            printf("目录不存在，程序退出。\n");
            return 0;
        }
        write_ini(ini_path, out);
        printf("配置完成。\n");
        return 1;
    }
    FILE *fp = fopen(ini_path, "r");
    if (!fp) die("无法读取 ini");
    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = 0;
        if (sscanf(line, " MAIN_PATH = \"%1023[^\"]\"", out) == 1 ||
            sscanf(line, " MAIN_PATH = %1023s", out) == 1) {
            strip_quotes(out);
            fclose(fp);
            if (!exists(out)) {
                printf("ini 中路径无效，请重新输入（留空退出）:\n> ");
                fflush(stdout);
                if (!fgets(out, out_len, stdin)) return 0;
                out[strcspn(out, "\r\n")] = 0;
                strip_quotes(out);
                if (out[0] == '\0') return 0;
                if (!exists(out)) {
                    printf("目录不存在，程序退出。\n");
                    return 0;
                }
                write_ini(ini_path, out);
                printf("配置完成。\n");
            }
            return 1;
        }
    }
    fclose(fp);
    printf("ini 格式错误，请重新输入（留空退出）:\n> ");
    fflush(stdout);
    if (!fgets(out, out_len, stdin)) return 0;
    out[strcspn(out, "\r\n")] = 0;
    strip_quotes(out);
    if (out[0] == '\0') return 0;
    if (!exists(out)) {
        printf("目录不存在，程序退出。\n");
        return 0;
    }
    write_ini(ini_path, out);
    printf("配置完成。\n");
    return 1;
}

/* ---------- 子命令 ---------- */
static void pip_install(int argc, char **argv, const char *root) {
    if (argc < 3) die("install 需要至少一个包名");
    const char *env_name = argv[2];
    char workdir[1024], cmd[4096];
    snprintf(workdir, sizeof(workdir), "%s%s%s", root, DIRSEP, env_name);
    if (_mkdir(workdir) != 0 && errno != EEXIST)
        die("创建目录失败");
    snprintf(cmd, sizeof(cmd),
             "cd /d \"%s\" && python -m venv venv && call venv\\Scripts\\activate && pip install %s",
             workdir, argv[2]);
    for (int i = 3; i < argc; ++i) {
        strcat(cmd, " ");
        strcat(cmd, argv[i]);
    }
    system(cmd);
}

static void software_run(int argc, char **argv, const char *root) {
    if (argc < 3) die("run 需要至少一个参数");
    const char *env_name = argv[2];
    char workdir[1024], cmd[4096];
    snprintf(workdir, sizeof(workdir), "%s%s%s", root, DIRSEP, env_name);
    if (!exists(workdir)) die("环境目录不存在");
    snprintf(cmd, sizeof(cmd),
             "cd /d \"%s\" && call venv\\Scripts\\activate && ", workdir);
    for (int i = 2; i < argc; ++i) {
        strcat(cmd, " ");
        strcat(cmd, argv[i]);
    }
    system(cmd);
}

static void software_uninstall(int argc, char **argv, const char *root) {
    if (argc < 3) die("uninstall 需要环境名");
    const char *env_name = argv[2];
    char path[1024];
    snprintf(path, sizeof(path), "%s%s%s", root, DIRSEP, env_name);
    if (!exists(path)) die("环境目录不存在");

    char cmd[2048];
    snprintf(cmd, sizeof(cmd), "rmdir /s /q \"%s\" >nul 2>&1", path);
    if (system(cmd) != 0)
        die("删除失败");
}

static void software_list(const char *root) {
    if (!exists(root)) die("根目录不存在");
    struct _finddata_t fd;
    intptr_t h;
    char pattern[1024];
    snprintf(pattern, sizeof(pattern), "%s%s*", root, DIRSEP);
    if ((h = _findfirst(pattern, &fd)) == -1) return;
    do {
        if (fd.name[0] != '.') puts(fd.name);
    } while (_findnext(h, &fd) == 0);
    _findclose(h);
}

/* ---------- 主函数 ---------- */
int main(int argc, char **argv) {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    if (argc < 2) {
        printf("用法: %s install|run|uninstall|list [参数...]\n", argv[0]);
        return 1;
    }
    char inipath[1024], root[1024];
    get_exe_dir(inipath, sizeof(inipath));
    strcat(inipath, "pipvenv.ini");
    if (!read_ini(inipath, root, sizeof(root))) return 1;
    if (strcmp(argv[1], "install")  == 0) pip_install(argc, argv, root);
    else if (strcmp(argv[1], "run") == 0) software_run(argc, argv, root);
    else if (strcmp(argv[1], "uninstall") == 0) software_uninstall(argc, argv, root);
    else if (strcmp(argv[1], "list") == 0) software_list(root);
    else printf("未知命令: %s\n", argv[1]);
    return 0;
}