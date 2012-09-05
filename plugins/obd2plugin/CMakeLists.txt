include(CheckIncludeFiles)
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

set(obd2sourceplugin_headers obd2source.h obdlib.h)
set(obd2sourceplugin_sources obd2source.cpp obdlib.cpp)
add_library(obd2sourceplugin MODULE ${obd2sourceplugin_sources})
set_target_properties(obd2sourceplugin PROPERTIES PREFIX "")
target_link_libraries(obd2sourceplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})

install(TARGETS obd2sourceplugin LIBRARY DESTINATION lib/automotive-message-broker)
