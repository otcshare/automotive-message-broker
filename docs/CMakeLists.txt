if(enable_docs)
  install (DIRECTORY amb DESTINATION ${DOC_INSTALL_DIR} COMPONENT Docs)
  install (DIRECTORY dbus DESTINATION ${DOC_INSTALL_DIR} COMPONENT Docs)
endif(enable_docs)
