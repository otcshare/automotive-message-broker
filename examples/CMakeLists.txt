set(amb_examples configwheel databaseconfig databasesource dbusconfig exampleconfig
        gpsnmea obdsourceconfig opencvluxconfig opencvdbusconfig qtmainloopconfig websocketsink2 websocketsource2)

install (FILES ${amb_examples} DESTINATION /etc/ambd/examples)

