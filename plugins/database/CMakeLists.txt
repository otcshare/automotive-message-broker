if(database_plugin)

include(CheckIncludeFiles)


include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${gio_INCLUDE_DIRS} ${gio-unix_INCLUDE_DIRS} )

set(databasesinkplugin_headers *.h )
set(databasesinkplugin_sources *.cpp)
add_library(databasesinkplugin MODULE ${databasesinkplugin_sources})
set_target_properties(databasesinkplugin PROPERTIES PREFIX "")
target_link_libraries(databasesinkplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} )

install(TARGETS databasesinkplugin LIBRARY DESTINATION lib/automotive-message-broker)

endif(database_plugin)
