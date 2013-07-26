
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

pkg_check_modules(gio REQUIRED gio-2.0)

set(dbussinkplugin_headers automotivemanager.h dbusplugin.h abstractproperty.h abstractdbusinterface.h dbusinterfacemanager.h runningstatus.h varianttype.h
custompropertyinterface.h uncategorizedproperty.h environmentproperties.h vehicleinfo.h maintenance.h parking.h fakeidlheader.h drivingsafety.h)
set(dbussinkplugin_sources automotivemanager.cpp dbusplugin.cpp abstractproperty.cpp abstractdbusinterface.cpp dbusinterfacemanager.cpp runningstatus.cpp
varianttype.cpp custompropertyinterface.cpp uncategorizedproperty.cpp environmentproperties.cpp vehicleinfo.cpp maintenance.cpp parking.cpp drivingsafety.cpp)

add_library(dbussinkplugin MODULE ${dbussinkplugin_sources})
set_target_properties(dbussinkplugin PROPERTIES PREFIX "")
target_link_libraries(dbussinkplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${gio_LIBRARIES})

install (TARGETS dbussinkplugin LIBRARY DESTINATION lib${LIB_SUFFIX}/automotive-message-broker)
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/amb.conf DESTINATION /etc/dbus-1/system.d )

add_subdirectory(amb-qt)
