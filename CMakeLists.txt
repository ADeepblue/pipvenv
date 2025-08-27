cmake_minimum_required(VERSION 3.10)
project(pipvenv C)  # 项目名为 pipvenv，语言为 C

set(CMAKE_C_STANDARD 11) # 设置 C 标准，根据你的需要调整

# 添加可执行目标，假设你的主源文件是 pipvenv.c
add_executable(pipvenv pipvenv.c)

# 如果你有多个源文件，可以这样添加
# add_executable(pipvenv main.c command_run.c command_install.c command_list.c utils.c)