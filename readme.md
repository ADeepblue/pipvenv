# pipvenv
一键虚拟环境 + 包管理 CLI  
*A tiny yet handy virtual-environment launcher for Windows*

---

## 🚀 功能
- **init**  初始化全局根目录
- **install** 一次性创建 venv 并安装任意包
- **run**  激活指定 venv 并执行任意命令
- **uninstall** 删除整个环境目录
- **list**  列出已创建的环境

---

## 📦 安装

| 方式        | 命令                                      |
|-------------|-----------------------------------------|
| **源码**    | `git@github.com:ADeepblue/pipvenv.git`  |

---

## 🚦 用法

```cmd

# 第一次运行，自动创建 pipvenv.ini
pipvenv install libretranslate

# 启动 jupyter
pipvenv run libretranslate

# 查看所有环境
pipvenv list

# 删除环境
pipvenv uninstall libretranslate
```
## 其他
此为个人写的小项目,代码比较简单,由python编写,AI转译C而来,欢迎各位大佬提issue和pr,谢谢!