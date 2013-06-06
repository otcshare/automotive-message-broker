if(bluemonkey_plugin)


find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Script REQUIRED)
if(Qt5Core_FOUND)
	message(STATUS "using Qt5")

	set(QT_INCLUDE_DIRS ${Qt5Core_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} ${Qt5Script_INCLUDE_DIRS})
	set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Network_LIBRARIES} ${Qt5Script_LIBRARIES})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS} -mcmodel=large")
	add_definitions(${Qt5Core_DEFINITIONS})

endif(Qt5Core_FOUND)

set(CMAKE_AUTOMOC ON)
set(communi_INCLUDE_DIRS /usr/include/qt4/Communi)
set(communi_LIBRARIES -lCommuni)
add_definitions(-DCOMMUNI_SHARED)

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${communi_INCLUDE_DIRS} ${QT_INCLUDE_DIRS})

set(bluemonkeyplugin_headers bluemonkey.h irccoms.h authenticate.h)
set(bluemonkeyplugin_sources bluemonkey.cpp irccoms.cpp authenticate.cpp)

add_library(bluemonkeyplugin MODULE ${bluemonkeyplugin_sources})
set_target_properties(bluemonkeyplugin PROPERTIES PREFIX "")
target_link_libraries(bluemonkeyplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${QT_LIBRARIES} ${communi_LIBRARIES})

install(TARGETS bluemonkeyplugin LIBRARY DESTINATION lib/automotive-message-broker)

endif(bluemonkey_plugin)
