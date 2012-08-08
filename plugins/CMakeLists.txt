
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

set(exampleplugin_headers exampleplugin.h)
set(exampleplugin_sources exampleplugin.cpp)
add_library(exampleplugin MODULE ${exampleplugin_sources})
set_target_properties(exampleplugin PROPERTIES PREFIX "")
target_link_libraries(exampleplugin -lamb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})

