if(gpsnmea_plugin)

set(gpsnmea_headers gpsnmea.h)
set(gpsnmea_sources gpsnmea.cpp)

add_library(gpsnmea MODULE ${gpsnmea_sources})
set_target_properties(gpsnmea PROPERTIES PREFIX "")
target_link_libraries(gpsnmea amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} boost_regex)

install(TARGETS gpsnmea LIBRARY DESTINATION lib/automotive-message-broker)

endif(gpsnmea_plugin)
