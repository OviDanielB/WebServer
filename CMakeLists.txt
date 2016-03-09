cmake_minimum_required(VERSION 3.4)
project(WebServer)

set(CMAKE_C_COMPILER "/usr/bin/gcc")

set(SOURCE_FILES main.c)

add_executable(WebServer ${SOURCE_FILES})