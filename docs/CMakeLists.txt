if(enable_docs)
	install (DIRECTORY amb DESTINATION ${DOC_INSTALL_DIR} COMPONENT Docs)
	install (DIRECTORY dbus DESTINATION ${DOC_INSTALL_DIR} COMPONENT Docs)
	install (DIRECTORY html DESTINATION ${DOC_INSTALL_DIR} COMPONENT Docs)
	install (DIRECTORY latex DESTINATION ${DOC_INSTALL_DIR} COMPONENT Docs)
	install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/amb.fidl DESTINATION ${DOC_INSTALL_DIR}/dbus/html/ COMPONENT Docs)
	add_custom_target(doc_idl ALL mkdir -p ${CMAKE_CURRENT_SOURCE_DIR}/dbus/html/ && cp ${CMAKE_CURRENT_SOURCE_DIR}/amb.fidl ${CMAKE_CURRENT_SOURCE_DIR}/dbus/html/amb.fidl WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMMENT "Moving amb.fidl" VERBATIM)

# add a target to generate API documentation with Doxygen
message(STATUS "doxygen doc generation enabled")
find_package(Doxygen)
if(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.idl.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.idl @ONLY)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.mainpage.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.mainpage @ONLY)
	add_custom_target(libamb_docs ALL ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs/amb COMMENT "Generating API documentation with Doxygen" VERBATIM)
	add_custom_target(idl ALL ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.idl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs/dbus COMMENT "Generating DBus API documentation with Doxygen" VERBATIM)
	add_custom_target(mainpage_docs ALL ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.mainpage WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs COMMENT "Generating mainpage documentation with Doxygen" VERBATIM)

	#generate mappings documentation
	configure_file (${CMAKE_CURRENT_SOURCE_DIR}/libamb.in ${CMAKE_CURRENT_BINARY_DIR}/libamb.h @ONLY)

	add_custom_target(genmappings_libamb
										python ${CMAKE_SOURCE_DIR}/tools/genmapping --output ${CMAKE_CURRENT_BINARY_DIR}/ambdbusmappings.idl ${dbus_mapping_headers}
										COMMENT "running genmapping")
	add_dependencies(libamb_docs genmappings_libamb)
	add_dependencies(idl genmappings_libamb)

endif(DOXYGEN_FOUND)


endif(enable_docs)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/amb.in.fidl ${CMAKE_CURRENT_SOURCE_DIR}/amb.fidl @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/mainpage.in.idl ${CMAKE_CURRENT_SOURCE_DIR}/mainpage.idl @ONLY)

