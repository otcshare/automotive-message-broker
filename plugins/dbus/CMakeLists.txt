
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

pkg_check_modules(gio REQUIRED gio-2.0)

set(dbussinkplugin_headers dbusplugin.h abstractproperty.h abstractdbusinterface.h dbusinterfacemanager.h accelerationproperty.h basicproperty.h properties.h)
set(dbussinkplugin_sources dbusplugin.cpp abstractproperty.cpp abstractdbusinterface.cpp dbusinterfacemanager.cpp accelerationproperty.cpp basicproperty.cpp properties.cpp)

add_library(dbussinkplugin MODULE ${dbussinkplugin_sources})
set_target_properties(dbussinkplugin PROPERTIES PREFIX "")
target_link_libraries(dbussinkplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${gio_LIBRARIES})

install (TARGETS dbussinkplugin LIBRARY DESTINATION lib/automotive-message-broker)
