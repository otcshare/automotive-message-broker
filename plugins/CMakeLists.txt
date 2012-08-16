
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

set(examplesourceplugin_headers exampleplugin.h)
set(examplesourceplugin_sources exampleplugin.cpp)

add_library(examplesourceplugin MODULE ${examplesourceplugin_sources})
set_target_properties(examplesourceplugin PROPERTIES PREFIX "")
target_link_libraries(examplesourceplugin -lamb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})


set(examplesinkplugin_headers examplesink.h)
set(examplesinkplugin_sources examplesink.cpp)

add_library(examplesinkplugin MODULE ${examplesinkplugin_sources})
set_target_properties(examplesinkplugin PROPERTIES PREFIX "")
target_link_libraries(examplesinkplugin -lamb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})