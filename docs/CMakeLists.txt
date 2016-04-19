if(enable_docs)


# add a target to generate API documentation with Doxygen
message(STATUS "doxygen doc generation enabled")
find_package(Doxygen)
if(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(docs ALL ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating API documentation with Doxygen" VERBATIM)

	#generate mappings documentation
	add_custom_target(genmappings_libamb
						python ${CMAKE_BINARY_DIR}/tools/genmapping --output ${CMAKE_CURRENT_BINARY_DIR}/ambdbusmappings.idl ${dbus_mapping_headers}
						COMMENT "running genmapping")
	add_dependencies(docs genmappings_libamb)

	install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} DESTINATION ${DOC_INSTALL_DIR} COMPONENT Docs)
endif(DOXYGEN_FOUND)


endif(enable_docs)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/libamb.in.h ${CMAKE_CURRENT_BINARY_DIR}/libamb.h @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/amb.in.fidl ${CMAKE_CURRENT_BINARY_DIR}/amb.fidl @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/mainpage.in.idl ${CMAKE_CURRENT_BINARY_DIR}/mainpage.idl @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/ambd-configuration.in.idl ${CMAKE_CURRENT_BINARY_DIR}/ambd-configuration.idl @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/ambd.in.md ${CMAKE_CURRENT_BINARY_DIR}/ambd.md @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/ambd.in.md ${CMAKE_SOURCE_DIR}/README.md @ONLY)
