
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

set(websocketsinkplugin_headers websocketsink.h websocketmanager.h)
set(websocketsinkplugin_sources websocketsinkmanager.cpp websocketsink.cpp)
add_library(websocketsinkplugin MODULE ${websocketsinkplugin_sources})
set_target_properties(websocketsinkplugin PROPERTIES PREFIX "")
target_link_libraries(websocketsinkplugin -lamb -lwebsockets  -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})
