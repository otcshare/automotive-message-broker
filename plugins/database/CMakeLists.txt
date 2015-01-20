if(database_plugin)

include(CheckIncludeFiles)

pkg_check_modules(sqlite REQUIRED sqlite3)

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${sqlite_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/plugins/common)

set(database_table_create "CREATE TABLE IF NOT EXISTS data (key TEXT, value BLOB, source TEXT, zone INTEGER, time REAL, sequence REAL, tripId TEXT)" CACHE INTERNAL "database plugin table create command")
add_definitions(-Ddatabase_table_create="${database_table_create}")

set(databasesinkplugin_headers databasesink.h utils.h basedb.hpp baseobject.h sqlitedatabase.h sqlitequery.h)
set(databasesinkplugin_sources databasesink.cpp utils.cpp sqlitedatabase.cpp sqlitequery.cpp basedb.hpp)
add_library(databasesinkplugin MODULE ${databasesinkplugin_sources})
set_target_properties(databasesinkplugin PROPERTIES PREFIX "")
target_link_libraries(databasesinkplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${sqlite_LIBRARIES} amb-plugins-common -L${CMAKE_CURRENT_BINARY_DIR}/plugins/common)

install(TARGETS databasesinkplugin LIBRARY DESTINATION ${PLUGIN_INSTALL_PATH})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/README ${CMAKE_CURRENT_BINARY_DIR}/database.README.md @ONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/database.in.json ${CMAKE_CURRENT_BINARY_DIR}/database @ONLY)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/database DESTINATION ${PLUGIN_SEGMENT_INSTALL_PATH})

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/database.README.md DESTINATION ${DOC_INSTALL_DIR}/plugins)

endif(database_plugin)
