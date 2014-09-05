if(test_plugin)

include(CheckIncludeFiles)


include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

set(testplugin_headers testplugin.h)
set(testplugin_sources testplugin.cpp)
add_library(testplugin MODULE ${testplugin_sources})
set_target_properties(testplugin PROPERTIES PREFIX "")
target_link_libraries(testplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})

install(TARGETS testplugin LIBRARY DESTINATION ${PLUGIN_INSTALL_PATH})

endif(test_plugin)
