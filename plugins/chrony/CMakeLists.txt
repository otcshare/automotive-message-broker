if(chrony_plugin)

set(chrony_headers chrony.h)
set(chrony_sources chrony.cpp )

add_library(chrony MODULE ${chrony_sources})
set_target_properties(chrony PROPERTIES PREFIX "")
target_link_libraries(chrony amb -L${CMAKE_CURRENT_BINARY_DIR}/lib  amb-plugins-common -L${CMAKE_CURRENT_BINARY_DIR}/plugins/common ${link_libraries} ${gio_LIBRARIES})

install(TARGETS chrony LIBRARY DESTINATION ${PLUGIN_INSTALL_PATH})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/README ${CMAKE_CURRENT_BINARY_DIR}/chrony.README @ONLY)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/chrony.README DESTINATION ${DOC_INSTALL_DIR}/plugins)

endif(chrony_plugin)
