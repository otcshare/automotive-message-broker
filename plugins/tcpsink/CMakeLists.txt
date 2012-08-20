
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

set(tcpsinkplugin_headers tcpsinkplugin.h)
set(tcpsinkplugin_sources tcpsinkmanager.cpp tcpsinkplugin.cpp)
add_library(tcpsinkplugin MODULE ${tcpsinkplugin_sources})
set_target_properties(tcpsinkplugin PROPERTIES PREFIX "")
target_link_libraries(tcpsinkplugin -lamb -lwebsockets  -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})