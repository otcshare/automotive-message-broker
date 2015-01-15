if(gpsnmea_plugin)

set(gpsnmea_headers gpsnmea.h)
set(gpsnmea_sources gpsnmea.cpp )

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${gio_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/plugins/common)

add_library(gpsnmea MODULE ${gpsnmea_sources})
set_target_properties(gpsnmea PROPERTIES PREFIX "")
target_link_libraries(gpsnmea amb -L${CMAKE_CURRENT_BINARY_DIR}/lib  amb-plugins-common -L${CMAKE_CURRENT_BINARY_DIR}/plugins/common ${link_libraries} ${gio_LIBRARIES})

install(TARGETS gpsnmea LIBRARY DESTINATION ${PLUGIN_INSTALL_PATH})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/README ${CMAKE_CURRENT_BINARY_DIR}/gpsnmea.README.md @ONLY)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/gpsnmea.README.md DESTINATION ${DOC_INSTALL_DIR}/plugins)

endif(gpsnmea_plugin)
