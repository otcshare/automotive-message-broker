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

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/configwheel.in ${CMAKE_CURRENT_BINARY_DIR}/configwheel @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/databasesource.in ${CMAKE_CURRENT_BINARY_DIR}/databasesource @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/dbusconfig.in ${CMAKE_CURRENT_BINARY_DIR}/dbusconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/exampleconfig.in ${CMAKE_CURRENT_BINARY_DIR}/exampleconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/gpsnmea.in ${CMAKE_CURRENT_BINARY_DIR}/gpsnmea @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/obdsourceconfig.in ${CMAKE_CURRENT_BINARY_DIR}/obdsourceconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/opencvluxconfig.in ${CMAKE_CURRENT_BINARY_DIR}/opencvluxconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/opencvdbusconfig.in ${CMAKE_CURRENT_BINARY_DIR}/opencvdbusconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/qtmainloopconfig.in ${CMAKE_CURRENT_BINARY_DIR}/qtmainloopconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/websocketsink2.in ${CMAKE_CURRENT_BINARY_DIR}/websocketsink2 @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/websocketsource2.in ${CMAKE_CURRENT_BINARY_DIR}/websocketsource2 @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/testsourceconfig.in ${CMAKE_CURRENT_BINARY_DIR}/testsourceconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cangenconfig.in ${CMAKE_CURRENT_BINARY_DIR}/cangenconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/bluemonkey/bluemonkeyconfig.in ${CMAKE_CURRENT_BINARY_DIR}/bluemonkey/bluemonkeyconfig @ONLY)

install (FILES ${amb_examples} DESTINATION /etc/ambd/examples)

