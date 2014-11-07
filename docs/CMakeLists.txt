if(enable_docs)
  install (DIRECTORY amb DESTINATION ${DOC_INSTALL_DIR} COMPONENT Docs)
  install (DIRECTORY dbus DESTINATION ${DOC_INSTALL_DIR} COMPONENT Docs)
  install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/amb.idl DESTINATION ${DOC_INSTALL_DIR}/dbus/html/ COMPONENT Docs)
  add_custom_target(doc_idl ALL mkdir -p ${CMAKE_CURRENT_SOURCE_DIR}/dbus/html/ && cp ${CMAKE_CURRENT_SOURCE_DIR}/amb.idl ${CMAKE_CURRENT_SOURCE_DIR}/dbus/html/amb.idl WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMMENT "Moving amb.idl" VERBATIM)

  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/amb.idl ${CMAKE_CURRENT_SOURCE_DIR}/amb.idl @ONLY)

endif(enable_docs)
