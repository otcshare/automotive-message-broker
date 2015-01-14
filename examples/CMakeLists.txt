set(amb_examples ${CMAKE_CURRENT_BINARY_DIR}/configwheel
				 ${CMAKE_CURRENT_BINARY_DIR}/databasesource
				 ${CMAKE_CURRENT_BINARY_DIR}/dbusconfig
				 ${CMAKE_CURRENT_BINARY_DIR}/exampleconfig
				 ${CMAKE_CURRENT_BINARY_DIR}/gpsnmea
				 ${CMAKE_CURRENT_BINARY_DIR}/obdsourceconfig
				 ${CMAKE_CURRENT_BINARY_DIR}/opencvluxconfig
				 ${CMAKE_CURRENT_BINARY_DIR}/opencvdbusconfig
				 ${CMAKE_CURRENT_BINARY_DIR}/qtmainloopconfig
				 ${CMAKE_CURRENT_BINARY_DIR}/websocketsink2
				 ${CMAKE_CURRENT_BINARY_DIR}/websocketsource2
				 ${CMAKE_CURRENT_BINARY_DIR}/testsourceconfig
				 ${CMAKE_CURRENT_BINARY_DIR}/bluemonkey/bluemonkeyconfig)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/configwheel.in.json ${CMAKE_CURRENT_BINARY_DIR}/configwheel @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/databasesource.in.json ${CMAKE_CURRENT_BINARY_DIR}/databasesource @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/databaselogging.in.json ${CMAKE_CURRENT_BINARY_DIR}/databaselogging @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/dbusconfig.in.json ${CMAKE_CURRENT_BINARY_DIR}/dbusconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/exampleconfig.in.json ${CMAKE_CURRENT_BINARY_DIR}/exampleconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/gpsnmea.in.json ${CMAKE_CURRENT_BINARY_DIR}/gpsnmea @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/obdsourceconfig.in.json ${CMAKE_CURRENT_BINARY_DIR}/obdsourceconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/opencvluxconfig.in.json ${CMAKE_CURRENT_BINARY_DIR}/opencvluxconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/opencvdbusconfig.in.json ${CMAKE_CURRENT_BINARY_DIR}/opencvdbusconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/qtmainloopconfig.in.json ${CMAKE_CURRENT_BINARY_DIR}/qtmainloopconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/websocketsink2.in.json ${CMAKE_CURRENT_BINARY_DIR}/websocketsink2 @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/websocketsource2.in.json ${CMAKE_CURRENT_BINARY_DIR}/websocketsource2 @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/databasewebsocketsink.in.json ${CMAKE_CURRENT_BINARY_DIR}/databasewebsocketsink @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/testsourceconfig.in.json ${CMAKE_CURRENT_BINARY_DIR}/testsourceconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cangenconfig.in.json ${CMAKE_CURRENT_BINARY_DIR}/cangenconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/bluemonkey/bluemonkeyconfig.in.json ${CMAKE_CURRENT_BINARY_DIR}/bluemonkey/bluemonkeyconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/testplugins.d.in.json ${CMAKE_CURRENT_SOURCE_DIR}/testplugins.d.json @ONLY)

install (FILES ${amb_examples} DESTINATION /etc/ambd/examples)

