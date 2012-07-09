
set(amb_sources steeringwheelangleproperty.cpp accelerationproperty.cpp enginecoolantproperty.cpp engineoilproperty.cpp fuelrangeproperty.cpp fuellevelproperty.cpp chime.cpp hornproperty.cpp interiorlightstatusproperty.cpp lightstatusproperty.cpp wheelbrakeproperty.cpp cruisecontrolstatus.cpp transmissiongearstatus.cpp tripmeterproperty.cpp fueleconomyproperty.cpp vehiclepowermodeproperty.cpp enginespeedproperty.cpp runningstatusinterface.cpp vehiclespeed.cpp abstractdbusinterface.cpp abstractproperty.cpp dbusinterfacemanager.cpp debugout.cpp)
set(amb_headers_install abstractproperty.h vehiclespeed.h hornproperty.h interiorlightstatusproperty.h lightstatusproperty.h wheelbrakeproperty.h cruisecontrolstatus.h transmissiongearstatus.h tripmeterproperty.h fueleconomyproperty.h vehiclepowermodeproperty.h enginespeedproperty.h chime.h)
include_directories( ${include_dirs} )
add_library(amb SHARED ${amb_sources})

target_link_libraries(amb ${libtool_LIBRARY} ${glib_LIBRARIES} ${gio_LIBRARIES})

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/automotive-message-broker.pc.in ${CMAKE_CURRENT_BINARY_DIR}/automotive-message-broker.pc @ONLY)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/automotive-message-broker.pc DESTINATION ${LIB_INSTALL_DIR}/${LIB_SUFFIX}/pkgconfig)
install (FILES ${amb_headers_install} DESTINATION ${INCLUDE_INSTALL_DIR}/amb COMPONENT Devel)
install (TARGETS amb LIBRARY DESTINATION ${LIB_INSTALL_DIR} RUNTIME DESTINATION bin ARCHIVE DESTINATION ${LIB_INSTALL_DIR})


