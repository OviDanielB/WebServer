cmake_minimum_required(VERSION 3.4)
project(WebServer)

set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_C_FLAGS " -pthread")


set(SOURCE_FILES web_server.c)

link_libraries(dl)
link_libraries(Magick)
link_libraries(Wand)
link_libraries(GraphicsMagickWand)

add_executable(db_helper DataBase/db_helper.c sqlite3.c DataBase/db_cache.c helper/helper.c)
add_executable(server web_server.c sqlite3.c constants.c DataBase/db_cache.c DataBase/db_helper.c
                Service/serveRequest.c Service/adaptImage.c Service/requestParser.c Service/responseWriter.c
                helper/calendar.c helper/helper.c helper/io_func.c helper/locking.c logging.c)