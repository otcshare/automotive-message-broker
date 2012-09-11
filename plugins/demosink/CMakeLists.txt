
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

pkg_check_modules(giounix REQUIRED gio-unix-2.0)
set(link_libraries ${link_libraries} ${giounix_LIBRARIES})
#set(include_dirs ${include_dirs} ${giounix_INCLUDE_DIRS})
include_directories(${giounix_INCLUDE_DIRS})

set(demosinkplugin_headers demosinkplugin.h)
set(demosinkplugin_sources demosinkplugin.cpp)

add_library(demosinkplugin MODULE ${demosinkplugin_sources})
set_target_properties(demosinkplugin PROPERTIES PREFIX "")
target_link_libraries(demosinkplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})

install(TARGETS demosinkplugin LIBRARY DESTINATION lib/automotive-message-broker)
