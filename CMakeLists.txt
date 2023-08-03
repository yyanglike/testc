cmake_minimum_required(VERSION 3.10)
project(my_project)

# 设置 GCC 或 Clang 编译器的字节对齐方式为 packed
add_compile_options(-fpack-struct)


set(CMAKE_C_STANDARD 11)

add_executable(my_executable main.c)
