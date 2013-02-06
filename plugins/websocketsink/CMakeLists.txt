include(CheckIncludeFiles)
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

find_library(websockets_LIBRARY websockets DOC "libwebsockets")

if(websockets_LIBRARY)
    message(STATUS "libwebsockets found")
else(websockets_LIBRARY)
    message(FATAL_ERROR "libwebsockets not found.  please install libwebsockets")
endif(websockets_LIBRARY)

set(websocketsinkplugin_headers websocketsink.h websocketmanager.h)
set(websocketsinkplugin_sources websocketsinkmanager.cpp websocketsink.cpp)
add_library(websocketsinkplugin MODULE ${websocketsinkplugin_sources})
set_target_properties(websocketsinkplugin PROPERTIES PREFIX "")
target_link_libraries(websocketsinkplugin amb ${websockets_LIBRARY} -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})

install(TARGETS websocketsinkplugin LIBRARY DESTINATION lib/automotive-message-broker)
