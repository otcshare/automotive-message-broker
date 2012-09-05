include(CheckIncludeFiles)
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

check_include_files(libwebsockets.h HAVE_WEBSOCKETS)

set(websocketsourceplugin_headers websocketsource.h)
set(websocketsourceplugin_sources websocketsource.cpp)
add_library(websocketsourceplugin MODULE ${websocketsourceplugin_sources})
set_target_properties(websocketsourceplugin PROPERTIES PREFIX "")
target_link_libraries(websocketsourceplugin -lamb -lwebsockets  -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})

install(TARGETS websocketsourceplugin LIBRARY DESTINATION lib/automotive-message-broker)
