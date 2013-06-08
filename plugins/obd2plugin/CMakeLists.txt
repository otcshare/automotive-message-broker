if(obd2_plugin)

include(CheckIncludeFiles)


pkg_check_modules(gio REQUIRED gio-2.0)
pkg_check_modules(gio-unix REQUIRED gio-unix-2.0)


include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${gio_INCLUDE_DIRS} ${gio-unix_INCLUDE_DIRS} )

set(obd2sourceplugin_headers obd2source.h obdlib.h obdpid.h bluetoothmanagerproxy.h  bluetoothadapterproxy.h bluetoothserialproxy.h asyncqueuewatcher.h)
set(obd2sourceplugin_sources obd2source.cpp obdlib.cpp obdpid.cpp bluetooth.hpp bluetoothmanagerproxy.c bluetoothadapterproxy.c bluetoothserialproxy.c asyncqueuewatcher.cpp)
add_library(obd2sourceplugin MODULE ${obd2sourceplugin_sources})
set_target_properties(obd2sourceplugin PROPERTIES PREFIX "")
target_link_libraries(obd2sourceplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${gio_LIBRARIES} ${gio-unix_LIBRARIES} )

install(TARGETS obd2sourceplugin LIBRARY DESTINATION lib${LIB_SUFFIX}/automotive-message-broker)

endif(obd2_plugin)
