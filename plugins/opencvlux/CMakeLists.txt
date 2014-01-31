if(opencvlux_plugin)

find_package(OpenCV REQUIRED)

if(OpenCV_LIBS)
	 message(STATUS "opencv found")
else(OpenCV_LIBS)
	 message(FATAL_ERROR "opencv missing.  please install opencv")
endif(OpenCV_LIBS)

option(ocl "enable opencl" OFF)

if(ocl)
	message(STATUS "found opencv ocl headers. enabling opencl support. ${ocl}")
	add_definitions(-DOPENCL)
else(ocl)
	message(STATUS "no opencl support")
endif(ocl)

option(cuda "enable cuda" OFF)

if(cuda)
	message(STATUS "found opencv cuda headers.  enabling cuda support. ${cuda}")
	add_definitions(-DCUDA)
else(cuda)
	message(STATUS "no opencv cuda headers found.  no cuda support")
	remove_definitions(-DCUDA)
endif(cuda)

find_package(Qt5Core REQUIRED)

if(Qt5Core_FOUND)
	message(STATUS "using Qt5")

	set(QT_INCLUDE_DIRS ${Qt5Core_INCLUDE_DIRS} )
	set(QT_LIBRARIES ${Qt5Core_LIBRARIES} )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
	message(STATUS "size of void_p: ${CMAKE_SIZEOF_VOID_P}")
	if(CMAKE_SIZEOF_VOID_P MATCHES "8")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcmodel=large")
	endif(CMAKE_SIZEOF_VOID_P MATCHES "8")
	add_definitions(${Qt5Core_DEFINITIONS})

endif(Qt5Core_FOUND)
set(CMAKE_AUTOMOC ON)

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${OpenCV_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${QT_INCLUDE_DIRS})

set(opencvluxplugin_headers opencvluxplugin.h)
set(opencvluxplugin_sources opencvluxplugin.cpp)

add_library(opencvluxplugin MODULE ${opencvluxplugin_sources})
set_target_properties(opencvluxplugin PROPERTIES PREFIX "")
target_link_libraries(opencvluxplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${OpenCV_LIBS} ${Boost_LIBRARIES} ${QT_LIBRARIES})

install(TARGETS opencvluxplugin LIBRARY DESTINATION lib${LIB_SUFFIX}/automotive-message-broker)

endif(opencvlux_plugin)
