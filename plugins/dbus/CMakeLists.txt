
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${CMAKE_SOURCE_DIR}/plugins/common)

pkg_check_modules(gio REQUIRED gio-2.0)

set(dbussinkplugin_headers automotivemanager.h dbusinterfacemanager.h runningstatus.h
	custompropertyinterface.h uncategorizedproperty.h environmentproperties.h vehicleinfo.h maintenance.h parking.h fakeidlheader.h drivingsafety.h personalization.h)
set(dbussinkplugin_sources automotivemanager.cpp dbusinterfacemanager.cpp runningstatus.cpp
	custompropertyinterface.cpp uncategorizedproperty.cpp environmentproperties.cpp vehicleinfo.cpp maintenance.cpp parking.cpp drivingsafety.cpp personalization.cpp)

add_library(dbussinkplugin MODULE ${dbussinkplugin_sources})
set_target_properties(dbussinkplugin PROPERTIES PREFIX "")
target_link_libraries(dbussinkplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${gio_LIBRARIES} -L${CMAKE_CURRENT_BINARY_DIR}/plugins/common amb-plugins-common)

install (TARGETS dbussinkplugin LIBRARY DESTINATION ${PLUGIN_INSTALL_PATH})
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/amb.conf DESTINATION /etc/dbus-1/system.d )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/amb.conf ${CMAKE_CURRENT_SOURCE_DIR}/amb.conf)

set(dbus_mapping_headers ${CMAKE_CURRENT_SOURCE_DIR}/runningstatus.h CACHE INTERNAL "dbus mapping headers")
set(dbus_mapping_headers ${dbus_mapping_headers} ${CMAKE_CURRENT_SOURCE_DIR}/environmentproperties.h CACHE INTERNAL "dbus mapping headers")
set(dbus_mapping_headers ${dbus_mapping_headers} ${CMAKE_CURRENT_SOURCE_DIR}/vehicleinfo.h CACHE INTERNAL "dbus mapping headers")
set(dbus_mapping_headers ${dbus_mapping_headers} ${CMAKE_CURRENT_SOURCE_DIR}/maintenance.h CACHE INTERNAL "dbus mapping headers")
set(dbus_mapping_headers ${dbus_mapping_headers} ${CMAKE_CURRENT_SOURCE_DIR}/parking.h CACHE INTERNAL "dbus mapping headers")
set(dbus_mapping_headers ${dbus_mapping_headers} ${CMAKE_CURRENT_SOURCE_DIR}/drivingsafety.h CACHE INTERNAL "dbus mapping headers")
set(dbus_mapping_headers ${dbus_mapping_headers} ${CMAKE_CURRENT_SOURCE_DIR}/personalization.h CACHE INTERNAL "dbus mapping headers")


configure_file(${CMAKE_CURRENT_SOURCE_DIR}/README ${CMAKE_CURRENT_BINARY_DIR}/dbus.README @ONLY)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/dbus.README DESTINATION ${DOC_INSTALL_DIR}/plugins)

add_subdirectory(amb-qt)
