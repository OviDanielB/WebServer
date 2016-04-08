cmake_minimum_required(VERSION 3.4)
project(WebServer)

set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_C_FLAGS " -pthread")


set(SOURCE_FILES web_server.c client.c)

link_libraries("dl")

add_executable(server web_server.c helper/io_func.c helper/io_func.h sqlite3.c helper/locking.c helper/locking.h
        Service/serveRequest.c Service/serveRequest.h Service/requestParser.c Service/requestParser.h Service/responseWriter.c
        Service/responseWriter.h helper/calendar.c helper/calendar.h constants.c constants.h Service/adaptImage.c Service/adaptImage.h)
add_executable(client client.c helper/io_func.c helper/io_func.h)