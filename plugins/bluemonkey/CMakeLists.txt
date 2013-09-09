if(bluemonkey_plugin)


find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Script REQUIRED)
if(Qt5Core_FOUND)
	message(STATUS "using Qt5")

	set(QT_INCLUDE_DIRS ${Qt5Core_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} ${Qt5Script_INCLUDE_DIRS})
	set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Network_LIBRARIES} ${Qt5Script_LIBRARIES})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
	message(STATUS "size of void_p: ${CMAKE_SIZEOF_VOID_P}")
	if(CMAKE_SIZEOF_VOID_P MATCHES "8")
		message(STATUS "can has 64bits")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcmodel=large")
	endif(CMAKE_SIZEOF_VOID_P MATCHES "8")
	add_definitions(${Qt5Core_DEFINITIONS})

endif(Qt5Core_FOUND)

set(CMAKE_AUTOMOC ON)
set(communi_INCLUDE_DIRS /usr/include/qt5/Communi)
set(communi_LIBRARIES -lCommuni)
add_definitions(-DCOMMUNI_SHARED)

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${communi_INCLUDE_DIRS} ${QT_INCLUDE_DIRS})

set(bluemonkeyplugin_headers bluemonkey.h irccoms.h authenticate.h agent.h)
set(bluemonkeyplugin_sources bluemonkey.cpp irccoms.cpp authenticate.cpp agent.cpp)

add_library(bluemonkeyplugin MODULE ${bluemonkeyplugin_sources})
set_target_properties(bluemonkeyplugin PROPERTIES PREFIX "")
target_link_libraries(bluemonkeyplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${QT_LIBRARIES} ${communi_LIBRARIES})

set(config_files ${CMAKE_CURRENT_SOURCE_DIR}/ircSettings.js
    ${CMAKE_CURRENT_SOURCE_DIR}/authSettings.js
    ${CMAKE_CURRENT_SOURCE_DIR}/config.js)

install(TARGETS bluemonkeyplugin LIBRARY DESTINATION lib/automotive-message-broker)
install (FILES ${config_files} DESTINATION /etc/ambd)
endif(bluemonkey_plugin)
