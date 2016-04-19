if(openxc_plugin)


message(STATUS "openxc protocol plugin enabled")

pkg_check_modules(json REQUIRED json)

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${json_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/plugins/common)

set(openxcplugin_headers openxcplugin.h)
set(openxcplugin_sources openxcplugin.cpp)

add_library(openxcplugin MODULE ${openxcplugin_sources})
set_target_properties(openxcplugin PROPERTIES PREFIX "")
target_link_libraries(openxcplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib amb-plugins-common -L${CMAKE_CURRENT_BINARY_DIR}/plugins/common ${link_libraries} ${json_LIBRARIES})

install(TARGETS openxcplugin LIBRARY DESTINATION ${PLUGIN_INSTALL_PATH})

endif(openxc_plugin)
