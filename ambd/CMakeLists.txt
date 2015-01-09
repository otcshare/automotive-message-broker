set(ambd_headers core.h imainloop.h pluginloader.h glibmainloop.h)
set(ambd_sources core.cpp main.cpp pluginloader.cpp glibmainloop.cpp imainloop.h)

if(qtmainloop)

	message(STATUS "using Qt mainloop")

	find_package(Qt5Core)
	if(Qt5Core_FOUND)
		set(QT_INCLUDE_DIRS ${Qt5Core_INCLUDE_DIRS})
		set(QT_LIBRARIES ${Qt5Core_LIBRARIES})
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
		add_definitions(${Qt5Core_DEFINITIONS})

		set(include_dirs ${include_dirs} ${QT_INCLUDE_DIRS})
		set(link_libraries ${link_libraries} ${QT_LIBRARIES})

		add_definitions(-DQT_NO_KEYWORDS)
		add_definitions(-DUSE_QT_CORE)

		set(qtmainloopplugin_headers qtmainloop.h)
		set(qtmainloopplugin_sources qtmainloop.cpp)

		add_library(qtmainloopplugin MODULE ${qtmainloopplugin_sources})
		set_target_properties(qtmainloopplugin PROPERTIES PREFIX "")
		target_link_libraries(qtmainloopplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})

		install(TARGETS qtmainloopplugin LIBRARY DESTINATION ${LIB_INSTALL_DIR}/automotive-message-broker)
	endif(Qt5Core_FOUND)
endif(qtmainloop)

add_executable(ambd ${ambd_sources})

include_directories(${include_dirs} )
target_link_libraries(ambd ${link_libraries} dl amb)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/config.in.json ${CMAKE_CURRENT_BINARY_DIR}/config @ONLY)

install (TARGETS ambd RUNTIME DESTINATION bin)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/config DESTINATION /etc/ambd)

