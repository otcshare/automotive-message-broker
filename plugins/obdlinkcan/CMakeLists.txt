if(obdlinkcan_plugin)

set(obdlinkcan_headers obdlinkcan.h obdlinkmx.h)
set(obdlinkcan_sources obdlinkcan.cpp obdlinkmx.cpp)

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${gio_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/plugins/common)

add_library(obdlinkcan MODULE ${obdlinkcan_sources})
set_target_properties(obdlinkcan PROPERTIES PREFIX "")
target_link_libraries(obdlinkcan amb -L${CMAKE_CURRENT_BINARY_DIR}/lib  amb-plugins-common -L${CMAKE_CURRENT_BINARY_DIR}/plugins/common ${link_libraries} boost_regex ${gio_LIBRARIES})

install(TARGETS obdlinkcan LIBRARY DESTINATION lib/automotive-message-broker)

endif(obdlinkcan_plugin)
