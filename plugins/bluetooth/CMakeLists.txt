if(bluetooth_plugin)

add_definitions(-Dbluetooth_plugin)

set(bluetooth_headers bluetoothplugin.h)
set(bluetooth_sources bluetoothplugin.cpp )

find_package(Qt5Core REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5Network REQUIRED)

message(STATUS ${Qt5DBus_INCLUDE_DIRS})

set(QT_INCLUDE_DIRS ${Qt5Core_INCLUDE_DIRS} ${Qt5DBus_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS})
set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5DBus_LIBRARIES} ${Qt5Network_LIBRARIES})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
add_definitions(${Qt5Core_DEFINITIONS})

set(CMAKE_AUTOMOC ON)

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${gio_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/plugins/common ${QT_INCLUDE_DIRS})

add_library(bluetoothplugin MODULE ${bluetooth_sources})
set_target_properties(bluetoothplugin PROPERTIES PREFIX "")
target_link_libraries(bluetoothplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib  amb-plugins-common -L${CMAKE_CURRENT_BINARY_DIR}/plugins/common ${link_libraries} ${gio_LIBRARIES} ${QT_LIBRARIES})

install(TARGETS bluetoothplugin LIBRARY DESTINATION ${PLUGIN_INSTALL_PATH})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/ambbt.conf DESTINATION /etc/dbus-1/system.d )

add_executable(testAmbBt testAmbBt.cpp bluetoothplugin.cpp)
target_link_libraries(testAmbBt ${link_libraries} amb -L${CMAKE_CURRENT_BINARY_DIR}/lib amb-plugins-common -L${CMAKE_CURRENT_BINARY_DIR}/plugins/common ${QT_LIBRARIES})

install (TARGETS testAmbBt RUNTIME DESTINATION bin)


configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bluetooth.in.json ${CMAKE_CURRENT_BINARY_DIR}/bluetooth @ONLY)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/bluetooth DESTINATION ${PLUGIN_SEGMENT_INSTALL_PATH})

endif(bluetooth_plugin)
