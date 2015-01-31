if(cangen_plugin)
############################################################################################################################################

pkg_check_modules(websockets REQUIRED libwebsockets)
pkg_check_modules(json REQUIRED json)

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${gio_INCLUDE_DIRS} ${gio-unix_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/plugins/common ${Boost_INCLUDE_DIRS} ${json_INCLUDE_DIRS})

set(cangenplugin_headers cangenplugin.h websockets.h)
set(cangenplugin_sources cangenplugin.cpp websockets.cpp)

add_library(cangenplugin MODULE ${cangenplugin_sources} ${cangenplugin_headers})
set_target_properties(cangenplugin PROPERTIES PREFIX "")
target_link_libraries(cangenplugin amb amb-plugins-common -L${CMAKE_CURRENT_BINARY_DIR}/plugins/common ${link_libraries} ${websockets_LIBRARIES} -lrt ${json_LIBRARIES})

install(TARGETS cangenplugin LIBRARY DESTINATION ${PLUGIN_INSTALL_PATH})

############################################################################################################################################

endif(cangen_plugin)
