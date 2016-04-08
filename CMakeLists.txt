cmake_minimum_required(VERSION 3.4)
project(WebServer)

set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_C_FLAGS " -pthread")


set(SOURCE_FILES web_server.c client.c)

link_libraries("dl")
link_libraries("Magick")
link_libraries("Wand")
link_libraries("GraphicsMagickWand")


add_executable(server web_server.c helper/io_func.c helper/io_func.h sqlite3.c helper/locking.c helper/locking.h
        constants.c constants.h  DataBase/db_cache.c DataBase/db_cache.h)
add_executable(client client.c helper/io_func.c helper/io_func.h)