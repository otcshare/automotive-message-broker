
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

set(wheelsourceplugin_headers wheelplugin.h)
set(wheelsourceplugin_sources wheelplugin.cpp)

add_library(wheelsourceplugin MODULE ${wheelsourceplugin_sources})
set_target_properties(wheelsourceplugin PROPERTIES PREFIX "")
target_link_libraries(wheelsourceplugin -lamb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})
