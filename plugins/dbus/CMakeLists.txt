
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

pkg_check_modules(gio REQUIRED gio-2.0)

set(dbussinkplugin_headers dbusplugin.h abstractproperty.h abstractdbusinterface.h dbusinterfacemanager.h basicproperty.h properties.h varianttype.h custompropertyinterface.h environmentproperties.h vehicleinfo.h)
set(dbussinkplugin_sources dbusplugin.cpp abstractproperty.cpp abstractdbusinterface.cpp dbusinterfacemanager.cpp basicproperty.cpp properties.cpp varianttype.cpp custompropertyinterface.cpp environmentproperties.cpp vehicleinfo.cpp)

add_library(dbussinkplugin MODULE ${dbussinkplugin_sources})
set_target_properties(dbussinkplugin PROPERTIES PREFIX "")
target_link_libraries(dbussinkplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${gio_LIBRARIES})

install (TARGETS dbussinkplugin LIBRARY DESTINATION lib/automotive-message-broker)
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/amb.conf DESTINATION /etc/dbus-1/system.d )

add_subdirectory(amb-qt)
