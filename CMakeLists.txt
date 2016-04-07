cmake_minimum_required(VERSION 3.4)
project(WebServer)

set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_C_FLAGS " -pthread")


set(SOURCE_FILES web_server.c client.c)

link_libraries("dl")


add_executable(server web_server.c io_func.c io_func.h sqlite3.c locking.c locking.h helper/calendar.c helper/calendar.h DataBase/db_helper.h DataBase/db_helper.c DataBase/db_cache.c DataBase/db_cache.h)
add_executable(client client.c io_func.c io_func.h)