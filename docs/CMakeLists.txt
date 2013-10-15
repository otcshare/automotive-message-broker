set(amb_docs drivingsafety.txt electricvehicle.txt environment.txt maintenance.txt parking.txt personalization.txt runningstatus.txt vehicleinfo.txt)

install (FILES ${amb_docs} DESTINATION ${DOC_INSTALL_DIR} COMPONENT Docs)

if(enable_docs)
  install (DIRECTORY html DESTINATION ${DOC_INSTALL_DIR} COMPONENT Docs)
endif(enable_docs)
