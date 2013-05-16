
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

set(examplesourceplugin_headers exampleplugin.h)
set(examplesourceplugin_sources exampleplugin.cpp)

add_library(examplesourceplugin MODULE ${examplesourceplugin_sources})
set_target_properties(examplesourceplugin PROPERTIES PREFIX "")
target_link_libraries(examplesourceplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})


set(examplesinkplugin_headers examplesink.h)
set(examplesinkplugin_sources examplesink.cpp)

add_library(examplesinkplugin MODULE ${examplesinkplugin_sources})
set_target_properties(examplesinkplugin PROPERTIES PREFIX "")
target_link_libraries(examplesinkplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})

install(TARGETS examplesinkplugin LIBRARY DESTINATION lib${LIB_SUFFIX}/automotive-message-broker)
install(TARGETS examplesourceplugin LIBRARY DESTINATION lib${LIB_SUFFIX}/automotive-message-broker)

add_subdirectory(wheel)
add_subdirectory(dbus)
add_subdirectory(websocketsink)
add_subdirectory(obd2plugin)
add_subdirectory(demosink)
add_subdirectory(websocketsourceplugin)
add_subdirectory(tpms)
add_subdirectory(database)
add_subdirectory(opencvlux)
add_subdirectory(gpsd)
add_subdirectory(murphyplugin)
add_subdirectory(bluemonkey)
