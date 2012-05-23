
set(libnobdy_sources runningstatusinterface.cpp propertytypes/vehiclespeed.cpp abstractdbusinterface.cpp propertytypes/abstractproperty.cpp dbusinterfacemanager.cpp pluginloader.cpp )
set(libnobdy_headers_install propertytypes/abstractproperty.h propertytypes/vehiclespeed.h)
add_library(libnobdy2 MODULE ${libnobdy_sources})
set_target_properties(libnobdy2 PROPERTIES PREFIX "")

target_link_libraries(libnobdy2 ${libtool_LIBRARY} ${glib_LIBRARIES} ${gio_LIBRARIES})

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/nobdy2.pc.in ${CMAKE_CURRENT_BINARY_DIR}/nobdy2.pc @ONLY)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/nobdy2.pc DESTINATION ${LIB_INSTALL_DIR}/${LIB_SUFFIX}/${CMAKE_LIBRARY_ARCHITECTURE}/pkgconfig)
install (FILES ${libnobdy_headers_install} DESTINATION ${INCLUDE_INSTALL_DIR}/nobdy2 COMPONENT Devel)
install (TARGETS libnobdy2 LIBRARY DESTINATION ${LIB_INSTALL_DIR} RUNTIME DESTINATION bin ARCHIVE DESTINATION ${LIB_INSTALL_DIR})


