if(websocketold_plugin)

include(CheckIncludeFiles)
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

pkg_check_modules(websockets REQUIRED libwebsockets)

set(websocketsinkplugin_headers websocketsink.h websocketmanager.h)
set(websocketsinkplugin_sources websocketsinkmanager.cpp websocketsink.cpp)
add_library(websocketsinkplugin MODULE ${websocketsinkplugin_sources})
set_target_properties(websocketsinkplugin PROPERTIES PREFIX "")
target_link_libraries(websocketsinkplugin amb ${websockets_LIBRARIES} -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})

install(TARGETS websocketsinkplugin LIBRARY DESTINATION ${PLUGIN_INSTALL_PATH})

endif(websocketold_plugin)
