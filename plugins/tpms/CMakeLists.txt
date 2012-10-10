include(CheckIncludeFiles)
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

check_include_files(libwebsockets.h HAVE_WEBSOCKETS)
set(CMAKE_CXX_FLAGS "-g")
set(tpmsplugin_headers tpmsplugin.h)
set(tpmsplugin_sources tpmsplugin.cpp)
add_library(tpmsplugin MODULE ${tpmsplugin_sources})
set_target_properties(tpmsplugin PROPERTIES PREFIX "")
target_link_libraries(tpmsplugin amb websockets  -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})

install(TARGETS tpmsplugin LIBRARY DESTINATION lib/automotive-message-broker)
