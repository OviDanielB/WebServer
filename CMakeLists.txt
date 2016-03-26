cmake_minimum_required(VERSION 3.4)
project(WebServer)

set(CMAKE_C_COMPILER "/usr/bin/gcc")

set(SOURCE_FILES web_server.c client.c)

add_executable(server web_server.c io_func.c io_func.h)
add_executable(client client.c io_func.c io_func.h)