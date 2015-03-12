set(plugins_common_sources abstractio.hpp serialport.hpp bluetoothadapterproxy.c bluetooth.hpp bluetoothmanagerproxy.c bluetoothserialproxy.c bluetooth5.cpp
		canadapter.cpp cansocket.cpp cansocketraw.cpp cansocketbcm.cpp cansocketreader.cpp canbusimpl.cpp cansocketadapter.cpp logger.cpp mutex.cpp
		thread.cpp dbusexport.cpp dbusplugin.cpp abstractdbusinterface.cpp dbussignaller.cpp varianttype.cpp)
set(plugins_common_headers_install abstractio.hpp serialport.hpp bluetooth.hpp bluetoothadapterproxy.h bluetoothmanagerproxy.h bluetoothserialproxy.h
		bluetooth5.h canframeinfo.h canbus.h canadapter.h cansocket.h cansocketraw.h cansocketbcm.h cansocketreader.h canbusimpl.h cansocketadapter.h
		canobserver.h logger.h mutex.h thread.h dbusexport.h dbusplugin.h abstractdbusinterface.h dbussignaller.h varianttype.h)

add_library(amb-plugins-common SHARED ${plugins_common_sources})

pkg_check_modules(gio-unix REQUIRED gio-unix-2.0)

include_directories( ${include_dirs} ${gio-unix_INCLUDE_DIRS} )

target_link_libraries(amb-plugins-common ${libtool_LIBRARY} ${glib_LIBRARIES} ${gio_LIBRARIES} ${gio-unix_LIBRARIES})

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/amb-plugins-common.pc.in ${CMAKE_CURRENT_BINARY_DIR}/amb-plugins-common.pc @ONLY)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/amb-plugins-common.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)

install (FILES ${plugins_common_headers_install} DESTINATION ${INCLUDE_INSTALL_DIR}/amb COMPONENT Devel)

install (TARGETS amb-plugins-common LIBRARY DESTINATION ${LIB_INSTALL_DIR} RUNTIME DESTINATION bin ARCHIVE DESTINATION ${LIB_INSTALL_DIR})


