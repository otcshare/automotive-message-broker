if(gpsd_plugin)


pkg_check_modules(gpsd REQUIRED libgps)

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${gpsd_INCLUDE_DIRS})

set(gpsdplugin_headers gpsdplugin.h)
set(gpsdplugin_sources gpsdplugin.cpp)

add_library(gpsdplugin MODULE ${gpsdplugin_sources})
set_target_properties(gpsdplugin PROPERTIES PREFIX "")
target_link_libraries(gpsdplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${gpsd_LIBRARIES})

install(TARGETS gpsdplugin LIBRARY DESTINATION lib/automotive-message-broker)

endif(gpsd_plugin)
