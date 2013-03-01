if(opencvlux_plugin)

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)
include(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

find_package(OpenCV REQUIRED)

if(OpenCV_LIBS)
	 message(STATUS "opencv found")
else(OpenCV_LIBS)
	 message(FATAL_ERROR "opencv missing.  please install opencv")
endif(OpenCV_LIBS)

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${OpenCV_INCLUDE_DIRS})

set(opencvluxplugin_headers opencvluxplugin.h)
set(opencvluxplugin_sources opencvluxplugin.cpp)

QT4_WRAP_CPP(opencvluxplugin_headers_moc ${opencvluxplugin_headers})

add_library(opencvluxplugin MODULE ${opencvluxplugin_sources} ${opencvluxplugin_headers_moc})
set_target_properties(opencvluxplugin PROPERTIES PREFIX "")
target_link_libraries(opencvluxplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${QT_LIBRARIES} ${OpenCV_LIBS})

install(TARGETS opencvluxplugin LIBRARY DESTINATION lib/automotive-message-broker)

endif(opencvlux_plugin)
