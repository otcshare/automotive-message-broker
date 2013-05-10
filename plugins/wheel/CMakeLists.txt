
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

pkg_check_modules(giounix REQUIRED gio-unix-2.0)
set(link_libraries ${link_libraries} ${giounix_LIBRARIES})
#set(include_dirs ${include_dirs} ${giounix_INCLUDE_DIRS})
include_directories(${giounix_INCLUDE_DIRS})

set(wheelsourceplugin_headers wheelplugin.h)
set(wheelsourceplugin_sources wheelplugin.cpp)

add_library(wheelsourceplugin MODULE ${wheelsourceplugin_sources})
set_target_properties(wheelsourceplugin PROPERTIES PREFIX "")
target_link_libraries(wheelsourceplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})

install(TARGETS wheelsourceplugin LIBRARY DESTINATION lib${LIB_SUFFIX}/automotive-message-broker)
