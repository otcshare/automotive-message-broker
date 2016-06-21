if(qt_bindings)

find_package(Qt5Core REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5DBus REQUIRED)

if(Qt5Core_FOUND)
	message(STATUS "using Qt5")

	set(QT_INCLUDE_DIRS ${Qt5Core_INCLUDE_DIRS} ${Qt5Quick_INCLUDE_DIRS} ${Qt5DBus_INCLUDE_DIRS})
	set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Quick_LIBRARIES} ${Qt5DBus_LIBRARIES})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
	add_definitions(${Qt5Core_DEFINITIONS})
	set(CMAKE_AUTOMOC ON)
endif(Qt5Core_FOUND)

set(ambqt_headers ambqt.h amb-qt_global.h)
set(ambqt_sources ambqt.cpp)
set(ambqtquick_sources ambqtquick.cpp)
set(ambqtquick_headers ambqtquick.h)

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${QT_INCLUDE_DIRS})

add_library(amb-qt SHARED ${ambqt_sources} ${ambqt_headers_moc})
add_library(ambqtquick MODULE ${ambqtquick_sources} ${ambqtquick_headers_moc})

target_link_libraries(amb-qt amb ${QT_LIBRARIES} -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${gio_LIBRARIES})
target_link_libraries(ambqtquick amb-qt ${QT_LIBRARIES} -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${gio_LIBRARIES})

add_executable(ambtestclient test.cpp)
target_link_libraries(ambtestclient amb amb-qt ${QT_LIBRARIES} -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${gio_LIBRARIES})

install (TARGETS amb-qt LIBRARY DESTINATION ${LIB_INSTALL_DIR})

install (FILES ${ambqt_headers} DESTINATION ${INCLUDE_INSTALL_DIR}/ COMPONENT Devel)
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/qmldir DESTINATION ${LIB_INSTALL_DIR}/qt5/qml/amb COMPONENT Devel)
install (TARGETS ambqtquick DESTINATION ${LIB_INSTALL_DIR}/qt5/qml/amb)
install (TARGETS ambtestclient RUNTIME DESTINATION bin)
endif(qt_bindings)
