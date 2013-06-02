#if(test_plugin)

include(CheckIncludeFiles)


pkg_check_modules(gio REQUIRED gio-2.0)
pkg_check_modules(gio-unix REQUIRED gio-unix-2.0)
#pkg_check_modules(glibmm REQUIRED glibmm-2.4)

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${gio_INCLUDE_DIRS} ${gio-unix_INCLUDE_DIRS} )

set(testplugin_headers testplugin.h)
set(testplugin_sources testplugin.cpp)
add_library(testplugin MODULE ${testplugin_sources})
set_target_properties(testplugin PROPERTIES PREFIX "")
target_link_libraries(testplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${gio_LIBRARIES} ${gio-unix_LIBRARIES} )

install(TARGETS testplugin LIBRARY DESTINATION lib${LIB_SUFFIX}/automotive-message-broker)

#endif(test_plugin)
