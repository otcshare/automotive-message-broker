
set(include_dirs ${CMAKE_CURRENT_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/lib ../ ../../../tests)

find_library(libtool_LIBRARY ltdl DOC "Libtool libraries")
find_path(libtool_INCLUDE_DIR ltdl.h DOC "Libtool headers")

pkg_check_modules(glib REQUIRED glib-2.0)
pkg_check_modules(json REQUIRED json)
pkg_check_modules(websockets REQUIRED libwebsockets)

#find libcanbus.a library:
find_library(canbus_LIBRARY canbus PATHS $ENV{IVIPOC_HOME}/native/lib DOC "canbus library")
if(canbus_LIBRARY)
     message(STATUS "canbus library found")
else(canbus_LIBRARY)
     message(FATAL_ERROR "canbus library is missing. please install or build canbus")
endif(canbus_LIBRARY)

#find libivipoc-common.a library:
find_library(ivipoc-common_LIBRARY ivipoc-common PATHS $ENV{IVIPOC_HOME}/native/lib DOC "ivipoc-common library")
if(ivipoc-common_LIBRARY)
     message(STATUS "ivipoc-common library found")
else(ivipoc-common_LIBRARY)
     message(FATAL_ERROR "ivipoc-common library is missing. please install or build canbus")
endif(ivipoc-common_LIBRARY)

#find canbus headers:
find_path(canbus_INCLUDE_DIR canbus.h canobserver.h logger.h mutex.h thread.h
    PATHS $ENV{IVIPOC_HOME}/native/include
    DOC "canbus headers")

if(canbus_INCLUDE_DIR)
    message(STATUS "canbus headers found.")
else(canbus_INCLUDE_DIR)
    message(FATAL_ERROR "no canbus headers found")
endif(canbus_INCLUDE_DIR)

set(ut_cangenplugin_headers cangenplugin.h websockets.h mockabstractroutingengine.h)
set(ut_cangenplugin_sources main.cpp ut_cangenplugin.cpp ../cangenplugin.cpp ../websockets.cpp)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
add_definitions(-fPIC -g)

add_executable(ut_cangenplugin ${ut_cangenplugin_sources})

include_directories(${include_dirs} ../ivipocbase ${libtool_INCLUDE_DIR} ${canbus_INCLUDE_DIR} ${glib_INCLUDE_DIRS} ${gio_INCLUDE_DIRS})
target_link_libraries(ut_cangenplugin ${link_libraries} ivipocbase -L${CMAKE_CURRENT_BINARY_DIR}/../../../ivipocbase/build ${libtool_LIBRARY} ${glib_LIBRARIES} ${json_LIBRARIES} amb ${websockets_LIBRARIES} ${canbus_LIBRARY} ${ivipoc-common_LIBRARY} -lgobject-2.0 -lpthread)

