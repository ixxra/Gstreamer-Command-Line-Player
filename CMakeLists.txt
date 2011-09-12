project(playbin)
cmake_minimum_required(VERSION 2.6)

set (GSTREAMER_INCLUDES /usr/include/gstreamer-0.10 /usr/include/glib-2.0 /usr/lib/glib-2.0/include 
     /usr/include/libxml2)
set (GSTREAMER_LIBS gstreamer-0.10 gobject-2.0 gmodule-2.0 xml2 gthread-2.0 rt glib-2.0)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${GSTREAMER_INCLUDES})

add_executable(gplay taglistdelegate.cpp gplay.cpp keysdelegate.cpp playbin.cpp main.cpp)
target_link_libraries(gplay ${GSTREAMER_LIBS})

add_executable(gstreamer-inspect gstreamer_inspect.c)
target_link_libraries(gstreamer-inspect ${GSTREAMER_LIBS})

install(TARGETS gplay RUNTIME DESTINATION bin)
