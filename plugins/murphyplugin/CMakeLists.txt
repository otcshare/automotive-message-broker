if(murphy_plugin)

include(CheckIncludeFiles)
include(FindPkgConfig)

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${MURPHY_COMMON_INCLUDE_DIRS})

pkg_check_modules(MURPHY_COMMON REQUIRED murphy-common)

#if(qtmainloop)
#    pkg_check_modules(MURPHY_QT REQUIRED murphy-qt)

	#set(include_dirs ${include_dirs} ${MURPHY_QT_INCLUDE_DIRS})
	#set(murphy_libraries ${murphy_libraries} ${MURPHY_QT_LIBRARIES})
#else(qtmainloop)
    pkg_check_modules(MURPHY_GLIB REQUIRED murphy-glib)

    set(include_dirs ${include_dirs} ${MURPHY_GLIB_INCLUDE_DIRS})
	set(murphy_libraries ${murphy_libraries} ${MURPHY_GLIB_LIBRARIES})
#endif(qtmainloop)


set(murphysourceplugin_headers murphysource.h)
set(murphysourceplugin_sources murphysource.cpp)

link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib ${MURPHY_COMMON_LIBRARY_DIRS})

add_library(murphysourceplugin MODULE ${murphysourceplugin_sources})
set_target_properties(murphysourceplugin PROPERTIES PREFIX "")
target_link_libraries(murphysourceplugin amb ${MURPHY_COMMON_LIBRARIES} -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${murphy_libraries})

install(TARGETS murphysourceplugin LIBRARY DESTINATION lib${LIB_SUFFIX}/automotive-message-broker)

endif(murphy_plugin)
