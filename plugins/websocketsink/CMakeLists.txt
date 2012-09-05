include(CheckIncludeFiles)
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

check_include_files(libwebsockets.h HAVE_WEBSOCKETS)
set(CMAKE_CXX_FLAGS "-g")
set(websocketsinkplugin_headers websocketsink.h websocketmanager.h)
set(websocketsinkplugin_sources websocketsinkmanager.cpp websocketsink.cpp)
add_library(websocketsinkplugin MODULE ${websocketsinkplugin_sources})
set_target_properties(websocketsinkplugin PROPERTIES PREFIX "")
target_link_libraries(websocketsinkplugin amb websockets  -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})

install(TARGETS websocketsinkplugin LIBRARY DESTINATION lib/automotive-message-broker)
