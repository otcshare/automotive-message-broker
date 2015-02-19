if(bluemonkey_plugin)

message(STATUS "enabling bluemonkey plugin")

set(qtmainloop ON CACHE INTERNAL "")
set(CMAKE_PREFIX_PATH "${QMAKE_INSTALL_PATH}")

message(STATUS "using path: ${CMAKE_PREFIX_PATH}")

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Qml REQUIRED)
if(Qt5Core_FOUND)
	message(STATUS "qt version: ${QMAKE_INSTALL_PATH}/include/QtCore/${Qt5Core_VERSION}/QtCore")
	set(QT_INCLUDE_DIRS ${Qt5Core_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} ${Qt5Qml_INCLUDE_DIRS})
	set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Network_LIBRARIES} ${Qt5Qml_LIBRARIES})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
	add_definitions(${Qt5Core_DEFINITIONS})
	add_definitions(-DQT_NO_KEYWORDS)
	add_definitions(-DUSE_QT_CORE)
endif(Qt5Core_FOUND)

set(CMAKE_AUTOMOC ON)

find_package(Qt5WebSockets QUIET)
if(Qt5WebSockets_FOUND)
	message(STATUS "enabling bluemonkey websocket module")
	add_executable(bluemonkeyWsModule bmws.cpp)
	set_target_properties(bluemonkeyWsModule PROPERTIES PREFIX "")
	set_target_properties(bluemonkeyWsModule PROPERTIES SUFFIX ".so")
	target_link_libraries(bluemonkeyWsModule ${link_libraries} ${QT_LIBRARIES} ${Qt5WebSockets_LIBRARIES})
	set(QT_INCLUDE_DIRS ${QT_INCLUDE_DIRS} ${Qt5WebSockets_LIBRARIES})
	install(TARGETS bluemonkeyWsModule RUNTIME DESTINATION ${PLUGIN_INSTALL_PATH})
endif()

find_library(communi NAMES Communi)

if(communi)
	message(STATUS "enabling irc bluemonkey module")
	set(communi_INCLUDE_DIRS /usr/include/qt5/Communi)
	set(communi_LIBRARIES -lCommuni)
	add_definitions(-DCOMMUNI_SHARED)

	add_library(bluemonkeyIrcModule MODULE irccoms.cpp)
	set_target_properties(bluemonkeyIrcModule PROPERTIES PREFIX "")
	target_link_libraries(bluemonkeyIrcModule ${link_libraries} amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${QT_LIBRARIES} ${communi_LIBRARIES})
	install(TARGETS bluemonkeyIrcModule LIBRARY DESTINATION ${PLUGIN_INSTALL_PATH})
endif(communi)

find_package(Qt5Sql QUIET)

if(Qt5Sql_FOUND)
	message(STATUS "enabling database bluemonkey module")
	add_library(bluemonkeyDbModule MODULE db.cpp)
	set_target_properties(bluemonkeyDbModule PROPERTIES PREFIX "")
	target_link_libraries(bluemonkeyDbModule ${link_libraries} ${QT_LIBRARIES} ${Qt5Sql_LIBRARIES})
	install(TARGETS bluemonkeyDbModule LIBRARY DESTINATION ${PLUGIN_INSTALL_PATH})
endif()

find_package(Qt5DBus QUIET)

if(Qt5DBus_FOUND)
	message(STATUS "enabling dbus bluemonkey module")
	add_executable(bluemonkeyDBusModule bmdbus.cpp)
	set_target_properties(bluemonkeyDBusModule PROPERTIES PREFIX "")
	set_target_properties(bluemonkeyDBusModule PROPERTIES SUFFIX ".so")
	target_link_libraries(bluemonkeyDBusModule ${link_libraries} ${QT_LIBRARIES} ${Qt5DBus_LIBRARIES})
	set(QT_INCLUDE_DIRS ${QT_INCLUDE_DIRS} ${Qt5DBus_INCLUDE_DIRS})
	install(TARGETS bluemonkeyDBusModule RUNTIME DESTINATION ${PLUGIN_INSTALL_PATH})
endif()

find_package(Qt5Bluetooth QUIET)

if(Qt5Bluetooth_FOUND)
	message(STATUS "enabling bluetooth LE bluemonkey module")
	add_executable(bluemonkeyBleModule ble.cpp)
	set_target_properties(bluemonkeyBleModule PROPERTIES PREFIX "")
	set_target_properties(bluemonkeyBleModule PROPERTIES SUFFIX ".so")
	target_link_libraries(bluemonkeyBleModule ${QT_LIBRARIES} ${Qt5Bluetooth_LIBRARIES})
	set(QT_INCLUDE_DIRS ${QT_INCLUDE_DIRS} ${Qt5DBus_INCLUDE_DIRS})
	install(TARGETS bluemonkeyBleModule RUNTIME DESTINATION ${PLUGIN_INSTALL_PATH})
endif()

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${communi_INCLUDE_DIRS} ${QT_INCLUDE_DIRS} ${QT_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/plugins/common)

add_executable(bluemonkey bluemonkeynode.cpp)
target_link_libraries(bluemonkey ${link_libraries} ${QT_LIBRARIES} bm)
install(TARGETS bluemonkey RUNTIME DESTINATION bin)

add_library(bm SHARED bluemonkey.cpp)
target_link_libraries(bm dl ${QT_LIBRARIES})
install(TARGETS bm LIBRARY DESTINATION ${LIB_INSTALL_DIR})
install (FILES bluemonkey.h DESTINATION ${INCLUDE_INSTALL_DIR}/amb COMPONENT Devel)

set(bluemonkeyplugin_headers amb.h authenticate.h)
set(bluemonkeyplugin_sources amb.cpp authenticate.cpp)

add_library(bluemonkeyplugin MODULE ${bluemonkeyplugin_sources})
set_target_properties(bluemonkeyplugin PROPERTIES PREFIX "")
target_link_libraries(bluemonkeyplugin dl amb bm -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${QT_LIBRARIES} ${communi_LIBRARIES} amb-plugins-common -L${CMAKE_CURRENT_BINARY_DIR}/plugins/common)

set(config_files ${CMAKE_CURRENT_BINARY_DIR}/config.js)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/config.js ${CMAKE_CURRENT_BINARY_DIR}/config.js @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/bmconfig.js ${CMAKE_CURRENT_BINARY_DIR}/bmconfig.js @ONLY)

install(TARGETS bluemonkeyplugin LIBRARY DESTINATION ${PLUGIN_INSTALL_PATH})
install (FILES ${config_files} DESTINATION /etc/ambd/bluemonkey)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/README ${CMAKE_CURRENT_BINARY_DIR}/bluemonkey.README.md @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bluemonkey.in.idl ${CMAKE_CURRENT_BINARY_DIR}/docs/bluemonkey.idl @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bluemonkey.in.json ${CMAKE_CURRENT_BINARY_DIR}/segment/bluemonkey @ONLY)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/segment/bluemonkey DESTINATION ${PLUGIN_SEGMENT_INSTALL_PATH})

set(bluemonkey_doc_files ${CMAKE_CURRENT_BINARY_DIR}/bluemonkey.README.md)

install (FILES ${bluemonkey_doc_files} DESTINATION ${DOC_INSTALL_DIR}/plugins)

endif(bluemonkey_plugin)
