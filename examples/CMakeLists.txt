set(amb_examples configwheel databasesource dbusconfig exampleconfig
		gpsnmea obdsourceconfig opencvluxconfig opencvdbusconfig qtmainloopconfig websocketsink2 websocketsource2
		testsourceconfig bluemonkey/bluemonkeyconfig)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/configwheel.in.json ${CMAKE_CURRENT_SOURCE_DIR}/configwheel @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/databasesource.in.json ${CMAKE_CURRENT_SOURCE_DIR}/databasesource @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/databaselogging.in.json ${CMAKE_CURRENT_SOURCE_DIR}/databaselogging @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/dbusconfig.in.json ${CMAKE_CURRENT_SOURCE_DIR}/dbusconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/exampleconfig.in.json ${CMAKE_CURRENT_SOURCE_DIR}/exampleconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/gpsnmea.in.json ${CMAKE_CURRENT_SOURCE_DIR}/gpsnmea @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/obdsourceconfig.in.json ${CMAKE_CURRENT_SOURCE_DIR}/obdsourceconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/opencvluxconfig.in.json ${CMAKE_CURRENT_SOURCE_DIR}/opencvluxconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/opencvdbusconfig.in.json ${CMAKE_CURRENT_SOURCE_DIR}/opencvdbusconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/qtmainloopconfig.in.json ${CMAKE_CURRENT_SOURCE_DIR}/qtmainloopconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/websocketsink2.in.json ${CMAKE_CURRENT_SOURCE_DIR}/websocketsink2 @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/websocketsource2.in.json ${CMAKE_CURRENT_SOURCE_DIR}/websocketsource2 @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/testsourceconfig.in.json ${CMAKE_CURRENT_SOURCE_DIR}/testsourceconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cangenconfig.in.json ${CMAKE_CURRENT_SOURCE_DIR}/cangenconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/bluemonkey/bluemonkeyconfig.in.json ${CMAKE_CURRENT_SOURCE_DIR}/bluemonkey/bluemonkeyconfig @ONLY)

install (FILES ${amb_examples} DESTINATION /etc/ambd/examples)

