if(websocketold_plugin)

include(CheckIncludeFiles)
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

pkg_check_modules(websockets REQUIRED libwebsockets)

set(websocketsourceplugin_headers websocketsource.h)
set(websocketsourceplugin_sources websocketsource.cpp)
add_library(websocketsourceplugin MODULE ${websocketsourceplugin_sources})
set_target_properties(websocketsourceplugin PROPERTIES PREFIX "")
target_link_libraries(websocketsourceplugin amb ${websockets_LIBRARIES} -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})

install(TARGETS websocketsourceplugin LIBRARY DESTINATION lib${LIB_SUFFIX}/automotive-message-broker)

endif(websocketold_plugin)
