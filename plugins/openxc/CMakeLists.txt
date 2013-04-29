if(openxc_plugin)


message(STATUS "openxc protocol plugin enabled")

pkg_check_modules(json REQUIRED json)

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${json_INCLUDE_DIRS})

set(openxcplugin_headers openxcplugin.h abastractio.hpp)
set(openxcplugin_sources openxcplugin.cpp bluetooth.hpp abstractio.hpp serialport.hpp)

add_library(openxcplugin MODULE ${openxcplugin_sources})
set_target_properties(openxcplugin PROPERTIES PREFIX "")
target_link_libraries(openxcplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${json_LIBRARIES})

install(TARGETS openxcplugin LIBRARY DESTINATION lib/automotive-message-broker)

endif(openxc_plugin)
