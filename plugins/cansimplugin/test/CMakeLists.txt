
set(include_dirs ${CMAKE_CURRENT_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/lib ../ ../../../tests)

find_library(libtool_LIBRARY ltdl DOC "Libtool libraries")
find_path(libtool_INCLUDE_DIR ltdl.h DOC "Libtool headers")

pkg_check_modules(glib REQUIRED glib-2.0)
pkg_check_modules(json REQUIRED json)

set(ut_cansimplugin_headers cansimplugin.h cansimpluginimpl.h mockabstractroutingengine.h)
set(ut_cansimplugin_sources main.cpp ut_cansimplugin.cpp ../cansimplugin.cpp)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
add_definitions(-fPIC -g)

add_executable(ut_cansimplugin ${ut_cansimplugin_sources})

include_directories(${include_dirs} ${libtool_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/plugins/common ${glib_INCLUDE_DIRS} ${gio_INCLUDE_DIRS})
target_link_libraries(ut_cansimplugin ${link_libraries} amb-plugins-common -L${CMAKE_CURRENT_BINARY_DIR}/plugins/common ${libtool_LIBRARY} ${glib_LIBRARIES} ${json_LIBRARIES} amb -lrt -lgobject-2.0 -lpthread)

