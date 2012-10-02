include(CheckIncludeFiles)


pkg_check_modules(gio REQUIRED gio-2.0)
pkg_check_modules(gio-unix REQUIRED gio-unix-2.0)
#pkg_check_modules(glibmm REQUIRED glibmm-2.4)

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${gio_INCLUDE_DIRS} ${gio-unix_INCLUDE_DIRS} )

set(obd2sourceplugin_headers obd2source.h obdlib.h bluetoothmanagerproxy.h  bluetoothadapterproxy.h bluetoothserialproxy.h )
set(obd2sourceplugin_sources obd2source.cpp obdlib.cpp bluetooth.hpp bluetoothmanagerproxy.c bluetoothadapterproxy.c bluetoothserialproxy.c)
add_library(obd2sourceplugin MODULE ${obd2sourceplugin_sources})
set_target_properties(obd2sourceplugin PROPERTIES PREFIX "")
target_link_libraries(obd2sourceplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${gio_LIBRARIES} ${gio-unix_LIBRARIES} )

install(TARGETS obd2sourceplugin LIBRARY DESTINATION lib/automotive-message-broker)
