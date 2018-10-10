cmake_minimum_required(VERSION 3.7)

project(
    shadowsocks
    VERSION 1.3.0
    LANGUAGES CXX
)

# 添加子目录 src
add_subdirectory(client)

add_subdirectory(libQss)
