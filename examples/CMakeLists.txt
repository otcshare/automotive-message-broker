set(amb_examples configwheel databaseconfig databasesource dbusconfig exampleconfig
        gpsnmea obdsourceconfig opencvluxconfig opencvdbusconfig qtmainloopconfig websocketsink2 websocketsource2
        testsourceconfig bluemonkey/bluemonkeyconfig)


configure_file (${CMAKE_CURRENT_SOURCE_DIR}/configwheel.in ${CMAKE_CURRENT_SOURCE_DIR}/configwheel @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/databaseconfig.in ${CMAKE_CURRENT_SOURCE_DIR}/databaseconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/databasesource.in ${CMAKE_CURRENT_SOURCE_DIR}/databasesource @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/dbusconfig.in ${CMAKE_CURRENT_SOURCE_DIR}/dbusconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/exampleconfig.in ${CMAKE_CURRENT_SOURCE_DIR}/exampleconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/gpsnmea.in ${CMAKE_CURRENT_SOURCE_DIR}/gpsnmea @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/obdsourceconfig.in ${CMAKE_CURRENT_SOURCE_DIR}/obdsourceconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/opencvluxconfig.in ${CMAKE_CURRENT_SOURCE_DIR}/opencvluxconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/opencvdbusconfig.in ${CMAKE_CURRENT_SOURCE_DIR}/opencvdbusconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/qtmainloopconfig.in ${CMAKE_CURRENT_SOURCE_DIR}/qtmainloopconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/websocketsink2.in ${CMAKE_CURRENT_SOURCE_DIR}/websocketsink2 @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/websocketsource2.in ${CMAKE_CURRENT_SOURCE_DIR}/websocketsource2 @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/testsourceconfig.in ${CMAKE_CURRENT_SOURCE_DIR}/testsourceconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/bluemonkey/bluemonkeyconfig.in ${CMAKE_CURRENT_SOURCE_DIR}/bluemonkey/bluemonkeyconfig @ONLY)


configure_file (${CMAKE_CURRENT_SOURCE_DIR}/configwheel.in ${CMAKE_CURRENT_SOURCE_DIR}/configwheel @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/databaseconfig.in ${CMAKE_CURRENT_SOURCE_DIR}/databaseconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/databasesource.in ${CMAKE_CURRENT_SOURCE_DIR}/databasesource @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/dbusconfig.in ${CMAKE_CURRENT_SOURCE_DIR}/dbusconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/exampleconfig.in ${CMAKE_CURRENT_SOURCE_DIR}/exampleconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/gpsnmea.in ${CMAKE_CURRENT_SOURCE_DIR}/gpsnmea @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/obdsourceconfig.in ${CMAKE_CURRENT_SOURCE_DIR}/obdsourceconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/opencvluxconfig.in ${CMAKE_CURRENT_SOURCE_DIR}/opencvluxconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/opencvdbusconfig.in ${CMAKE_CURRENT_SOURCE_DIR}/opencvdbusconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/qtmainloopconfig.in ${CMAKE_CURRENT_SOURCE_DIR}/qtmainloopconfig @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/websocketsink2.in ${CMAKE_CURRENT_SOURCE_DIR}/websocketsink2 @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/websocketsource2.in ${CMAKE_CURRENT_SOURCE_DIR}/websocketsource2 @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/testsourceconfig.in ${CMAKE_CURRENT_SOURCE_DIR}/testsourceconfig @ONLY)

install (FILES ${amb_examples} DESTINATION /etc/ambd/examples)

