if(database_plugin)

include(CheckIncludeFiles)

pkg_check_modules(sqlite REQUIRED sqlite3)

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${sqlite_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/plugins/common)

set(databasesinkplugin_headers databasesink.h utils.h basedb.hpp baseobject.h sqlitedatabase.h sqlitequery.h)
set(databasesinkplugin_sources databasesink.cpp utils.cpp sqlitedatabase.cpp sqlitequery.cpp basedb.hpp)
add_library(databasesinkplugin MODULE ${databasesinkplugin_sources})
set_target_properties(databasesinkplugin PROPERTIES PREFIX "")
target_link_libraries(databasesinkplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${sqlite_LIBRARIES} amb-plugins-common -L${CMAKE_CURRENT_BINARY_DIR}/plugins/common)

install(TARGETS databasesinkplugin LIBRARY DESTINATION ${PLUGIN_INSTALL_PATH})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/README ${CMAKE_CURRENT_SOURCE_DIR}/README @ONLY)

endif(database_plugin)
