if(websocket_plugin)

add_definitions(-Dwebsocket_plugin)

include(CheckIncludeFiles)
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

find_package(Qt5Core REQUIRED)

if(Qt5Core_FOUND)
  set(QT_INCLUDE_DIRS ${Qt5Core_INCLUDE_DIRS} )
  set(QT_LIBRARIES ${Qt5Core_LIBRARIES} )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
  add_definitions(${Qt5Core_DEFINITIONS} -DQTBINARY_DATA)
  add_definitions(-DQT_NO_KEYWORDS)
  set(CMAKE_AUTOMOC ON)
endif(Qt5Core_FOUND)

pkg_check_modules(websockets REQUIRED libwebsockets)

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${QT_INCLUDE_DIRS})

set(websocketsink_headers websocketsink.h websocketmanager.h common.h)
set(websocketsink_sources websocketsinkmanager.cpp websocketsink.cpp common.cpp)
add_library(websocketsink MODULE ${websocketsink_sources})
set_target_properties(websocketsink PROPERTIES PREFIX "")
target_link_libraries(websocketsink amb ${websockets_LIBRARIES} -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${QT_LIBRARIES})

install(TARGETS websocketsink LIBRARY DESTINATION ${PLUGIN_INSTALL_PATH})


set(websocketsource_headers websocketsource.h common.h)
set(websocketsource_sources websocketsource.cpp common.cpp)
add_library(websocketsource MODULE ${websocketsource_sources})
set_target_properties(websocketsource PROPERTIES PREFIX "")
target_link_libraries(websocketsource amb ${websockets_LIBRARIES} -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${QT_LIBRARIES})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test/vehicle.js ${CMAKE_CURRENT_SOURCE_DIR}/test/vehicle.js)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test/test.js ${CMAKE_CURRENT_SOURCE_DIR}/test/test.js)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test/events.js ${CMAKE_CURRENT_SOURCE_DIR}/test/events.js)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test/index.html ${CMAKE_CURRENT_SOURCE_DIR}/test/index.html)

install(TARGETS websocketsource LIBRARY DESTINATION ${PLUGIN_INSTALL_PATH})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/README ${CMAKE_CURRENT_BINARY_DIR}/websocket.README @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/protocol.idl ${CMAKE_CURRENT_BINARY_DIR}/docs/protocol.idl @ONLY)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/websocket.README DESTINATION ${DOC_INSTALL_DIR}/plugins)

if(enable_docs)
	find_package(Doxygen)
	if(DOXYGEN_FOUND)
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
		add_custom_target(websocket_docs ALL ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
											COMMENT "Generating Websocket protocol documentation with Doxygen" VERBATIM)

		install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION ${DOC_INSTALL_DIR}/plugins/websocket COMPONENT Docs)
		install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test DESTINATION ${DOC_INSTALL_DIR}/plugins/websocket COMPONENT Docs)
	endif(DOXYGEN_FOUND)

endif(enable_docs)

endif(websocket_plugin)
