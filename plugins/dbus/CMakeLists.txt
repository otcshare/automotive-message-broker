
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

set(dbussinkplugin_headers dbusplugin.h)
set(dbussinkplugin_sources dbusplugin.cpp)

add_library(dbussinkplugin MODULE ${dbussinkplugin_sources})
set_target_properties(dbussinkplugin PROPERTIES PREFIX "")
target_link_libraries(dbussinkplugin -lamb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})


