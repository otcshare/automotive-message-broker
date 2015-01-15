
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

install(TARGETS examplesinkplugin LIBRARY DESTINATION ${PLUGIN_INSTALL_PATH})
install(TARGETS examplesourceplugin LIBRARY DESTINATION ${PLUGIN_INSTALL_PATH})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/exampleplugins.README.txt ${CMAKE_CURRENT_BINARY_DIR}/exampleplugins.README @ONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/examplesource.in.json ${CMAKE_CURRENT_BINARY_DIR}/examplesource @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/examplesink.in.json ${CMAKE_CURRENT_BINARY_DIR}/examplesink @ONLY)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/exampleplugins.README DESTINATION ${DOC_INSTALL_DIR}/plugins)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/examplesource DESTINATION ${PLUGIN_SEGMENT_INSTALL_PATH})
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/examplesink DESTINATION ${PLUGIN_SEGMENT_INSTALL_PATH})


if(enable_docs)
	find_package(Doxygen)
	if(DOXYGEN_FOUND)
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/plugins.in.idl ${CMAKE_CURRENT_BINARY_DIR}/plugins.idl @ONLY)
		add_custom_target(plugin_docs ALL ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
											COMMENT "Generating plugin documentation with Doxygen" VERBATIM)

		install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION ${DOC_INSTALL_DIR}/plugins COMPONENT Docs)
	endif(DOXYGEN_FOUND)

endif(enable_docs)

add_subdirectory(common)

add_subdirectory(wheel)
add_subdirectory(dbus)
add_subdirectory(websocket)
add_subdirectory(obd2plugin)
add_subdirectory(demosink)
add_subdirectory(database)
add_subdirectory(opencvlux)
add_subdirectory(murphyplugin)
add_subdirectory(testplugin)
add_subdirectory(bluemonkey)
add_subdirectory(gpsnmea)
add_subdirectory(openxc)
add_subdirectory(bluetooth)
add_subdirectory(cansimplugin)
add_subdirectory(cangenplugin)
