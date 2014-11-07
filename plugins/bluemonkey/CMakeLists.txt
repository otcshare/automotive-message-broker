if(bluemonkey_plugin)

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Qml REQUIRED)
if(Qt5Core_FOUND)
  message(STATUS "using Qt5")

  set(QT_INCLUDE_DIRS ${Qt5Core_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} ${Qt5Qml_INCLUDE_DIRS})
  set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Network_LIBRARIES} ${Qt5Qml_LIBRARIES})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
  add_definitions(${Qt5Core_DEFINITIONS})

endif(Qt5Core_FOUND)

set(CMAKE_AUTOMOC ON)

find_library(communi NAMES Communi)

if(communi)
  message(STATUS "enabling irc bluemonkey module")
  set(communi_INCLUDE_DIRS /usr/include/qt5/Communi)
  set(communi_LIBRARIES -lCommuni)
  add_definitions(-DCOMMUNI_SHARED)

  add_library(bluemonkeyIrcModule MODULE irccoms.cpp)
  set_target_properties(bluemonkeyIrcModule PROPERTIES PREFIX "")
  target_link_libraries(bluemonkeyIrcModule ${link_libraries} ${QT_LIBRARIES} ${communi_LIBRARIES})
  install(TARGETS bluemonkeyIrcModule LIBRARY DESTINATION ${PLUGIN_INSTALL_PATH})
endif(communi)


include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${communi_INCLUDE_DIRS} ${QT_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/plugins/common)

set(bluemonkeyplugin_headers bluemonkey.h authenticate.h)
set(bluemonkeyplugin_sources bluemonkey.cpp authenticate.cpp)

add_library(bluemonkeyplugin MODULE ${bluemonkeyplugin_sources})
set_target_properties(bluemonkeyplugin PROPERTIES PREFIX "")
target_link_libraries(bluemonkeyplugin dl amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${QT_LIBRARIES} ${communi_LIBRARIES} amb-plugins-common -L${CMAKE_CURRENT_BINARY_DIR}/plugins/common)

set(config_files ${CMAKE_CURRENT_SOURCE_DIR}/config.js)

install(TARGETS bluemonkeyplugin LIBRARY DESTINATION ${PLUGIN_INSTALL_PATH})
install (FILES ${config_files} DESTINATION /etc/ambd/bluemonkey)
endif(bluemonkey_plugin)
