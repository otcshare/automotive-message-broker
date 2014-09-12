if(cansim_plugin)
############################################################################################################################################

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${gio_INCLUDE_DIRS} ${gio-unix_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/plugins/common ${Boost_INCLUDE_DIRS} ${json_INCLUDE_DIRS})

set(cansimplugin_headers cansimplugin.h)
set(cansimplugin_sources cansimplugin.cpp)

add_library(cansimplugin MODULE ${cansimplugin_sources} ${cansimplugin_headers})
set_target_properties(cansimplugin PROPERTIES PREFIX "")
target_link_libraries(cansimplugin amb amb-plugins-common -L${CMAKE_CURRENT_BINARY_DIR}/plugins/common ${link_libraries} -lrt)

install(TARGETS cansimplugin LIBRARY DESTINATION ${PLUGIN_INSTALL_PATH})

############################################################################################################################################

endif(cansim_plugin)

