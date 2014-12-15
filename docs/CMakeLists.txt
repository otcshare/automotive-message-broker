if(enable_docs)
  install (DIRECTORY amb DESTINATION ${DOC_INSTALL_DIR} COMPONENT Docs)
  install (DIRECTORY dbus DESTINATION ${DOC_INSTALL_DIR} COMPONENT Docs)
  install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/amb.fidl DESTINATION ${DOC_INSTALL_DIR}/dbus/html/ COMPONENT Docs)
  add_custom_target(doc_idl ALL mkdir -p ${CMAKE_CURRENT_SOURCE_DIR}/dbus/html/ && cp ${CMAKE_CURRENT_SOURCE_DIR}/amb.fidl ${CMAKE_CURRENT_SOURCE_DIR}/dbus/html/amb.fidl WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMMENT "Moving amb.fidl" VERBATIM)

endif(enable_docs)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/amb.in.idl ${CMAKE_CURRENT_SOURCE_DIR}/amb.fidl @ONLY)
