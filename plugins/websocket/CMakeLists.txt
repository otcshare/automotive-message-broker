if(websocket_plugin)

add_definitions(-Dwebsocket_plugin)

include(CheckIncludeFiles)
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

find_package(Qt5Core REQUIRED)

if(Qt5Core_FOUND)
	message(STATUS "using Qt5")

	set(QT_INCLUDE_DIRS ${Qt5Core_INCLUDE_DIRS} )
	set(QT_LIBRARIES ${Qt5Core_LIBRARIES} )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
	message(STATUS "size of void_p: ${CMAKE_SIZEOF_VOID_P}")
	if(CMAKE_SIZEOF_VOID_P MATCHES "8")
		message(STATUS "64bit enabled")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcmodel=large")
	endif(CMAKE_SIZEOF_VOID_P MATCHES "8")
	add_definitions(${Qt5Core_DEFINITIONS} -DQTBINARY_DATA)
	set(CMAKE_AUTOMOC ON)
endif(Qt5Core_FOUND)

pkg_check_modules(websockets REQUIRED libwebsockets)

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${QT_INCLUDE_DIRS})

set(websocketsink_headers websocketsink.h websocketmanager.h common.h)
set(websocketsink_sources websocketsinkmanager.cpp websocketsink.cpp common.cpp)
add_library(websocketsink MODULE ${websocketsink_sources})
set_target_properties(websocketsink PROPERTIES PREFIX "")
target_link_libraries(websocketsink amb ${websockets_LIBRARIES} -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${QT_LIBRARIES})

install(TARGETS websocketsink LIBRARY DESTINATION lib${LIB_SUFFIX}/automotive-message-broker)


set(websocketsource_headers websocketsource.h common.h)
set(websocketsource_sources websocketsource.cpp common.cpp)
add_library(websocketsource MODULE ${websocketsource_sources})
set_target_properties(websocketsource PROPERTIES PREFIX "")
target_link_libraries(websocketsource amb ${websockets_LIBRARIES} -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${QT_LIBRARIES})

install(TARGETS websocketsource LIBRARY DESTINATION lib${LIB_SUFFIX}/automotive-message-broker)

endif(websocket_plugin)
