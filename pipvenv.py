# -*- coding =utf-8 -*-
# @Time :2025/8/25 上午10:45
# @Author : deepblue
# @Files:pipvenv.py
# @Software: PyCharm
import re
import sys
import os
import subprocess
import shutil

def pip_install(argv:list,target_path):
    temp_list = argv.copy()
    del temp_list[0:2]
    os.chdir(target_path)

    try:
        os.makedirs(temp_list[0],mode=0o777)
    except FileExistsError:
        print("folder has been created")
        sys.exit()

    os.chdir(temp_list[0])

    command = "python -m venv venv " + "&&" + " call .\\venv\\Scripts\\activate" + "&&" + \
        "pip install " + " ".join(temp_list)
    subprocess.run(command,shell=True)
    sys.exit()

def software_run(argv:list,target_path):
    temp_list = argv.copy()
    del temp_list[0:2]
    try:
        os.chdir(target_path+"\\"+temp_list[0])
        command = " call .\\venv\\Scripts\\activate" + "&&" + " ".join(temp_list)
        subprocess.run(command,shell=True)
        sys.exit()
    except FileNotFoundError as E:
        print("The folder is not exist.")
        sys.exit()

def software_uninstall(argv:list,target_path):
    key = argv[2]
    try:
        os.chdir(target_path)
        shutil.rmtree(key)
        sys.exit()
    except FileNotFoundError as E:
        print("The folder is not exist.")
        sys.exit()

def software_list(target_path):
    os.chdir(target_path)
    for item in os.listdir():
        print(item)

def writeini(visualPath:str):
    with open("other/pipvenv.ini", 'w', encoding="utf-8") as file:
        file.write(f"MAIN_PATH=\"{visualPath}\"\n")


def readini(IniPath:str):
    # if there is no pipvenv.ini
    if os.path.exists(IniPath) == False:
        visualPath = input("please input visual environment path:")
        # if input path is not correct
        if os.path.isdir(visualPath) == False:
            print("The path you input is invalid path, the program will exit, you can run exe again or put the visual environment path in pipvenv.ini, and it will be automatically created.")
            # create a file and close
            writeini("")
            sys.exit()
        else:
            # path is correct
            writeini(visualPath)
            print("config init finished")
            return visualPath
    else:
        # if there is pipvenv.ini
        with open(IniPath, 'r') as file:
            Pathtext = file.read()
            pattern = "MAIN_PATH=\'([^\']*)\'\n|MAIN_PATH=\"([^\']+)\"\n"
            # check format
            if re.match(pattern=pattern,string=Pathtext):
                # get Path text
                Path_dict = {}
                exec(Pathtext,locals(),Path_dict)
                Path = Path_dict["MAIN_PATH"]
                if os.path.isdir(Path_dict["MAIN_PATH"]) == False:
                    visualPath = input("please input visual environment path, the existed Path environment is invalid.\nnew:")
                    if os.path.isdir(visualPath) == False:
                        print("The path you input is invalid path, the program will exit, you can run exe again or put the visual environment path in pipvenv.ini, and it will be automatically created.")
                        # create a file and close
                        writeini("")
                        sys.exit()
                    else:
                        # path is correct
                        writeini(visualPath)
                        print("config init finished")
                        return visualPath
                else:
                    return Path


            # invaild file format
            else:
                visualPath = input("please input visual environment path, the Path environment is invalid.\nnew:")
                if os.path.isdir(visualPath) == False:
                    # invalid path
                    print(
                        "The path you input is invalid path, the program will exit, you can run exe again or put the visual environment path in pipvenv.ini, and it will be automatically created.")
                    # clear the file and
                    writeini("")
                    sys.exit()
                else:
                    # valid path
                    writeini(visualPath)
                    print("config init finished")
                    return visualPath


def main():

    # init
    IniPath = os.getcwd()+"\\pipvenv.ini"

    if (len(sys.argv) < 3) & (sys.argv[1] != "list"):
        print("too less argv for pipvenv.py")
        sys.exit()

    Path_text = readini(IniPath)

    if sys.argv[1] == "install":
        pip_install(sys.argv,Path_text)
    if sys.argv[1] == "run":
        software_run(sys.argv,Path_text)
    if sys.argv[1] == "uninstall":
        software_uninstall(sys.argv,Path_text)
    if sys.argv[1] == "list":
        software_list(Path_text)

if __name__ == '__main__':
    main()
