if(gpsnmea_plugin)

set(gpsnmea_headers gpsnmea.h)
set(gpsnmea_sources gpsnmea.cpp abstractio.hpp serialport.hpp bluetoothadapterproxy.c bluetooth.hpp bluetoothmanagerproxy.c bluetoothserialproxy.c)

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${gio_INCLUDE_DIRS} ${gio-unix_INCLUDE_DIRS})

pkg_check_modules(gio REQUIRED gio-2.0)
pkg_check_modules(gio-unix REQUIRED gio-unix-2.0)

add_library(gpsnmea MODULE ${gpsnmea_sources})
set_target_properties(gpsnmea PROPERTIES PREFIX "")
target_link_libraries(gpsnmea amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} boost_regex ${gio_LIBRARIES} ${gio-unix_LIBRARIES})

install(TARGETS gpsnmea LIBRARY DESTINATION lib/automotive-message-broker)

endif(gpsnmea_plugin)
