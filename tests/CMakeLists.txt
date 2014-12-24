set(amb_tests ${CMAKE_CURRENT_BINARY_DIR}/amb-get ${CMAKE_CURRENT_BINARY_DIR}/amb-set
    ${CMAKE_CURRENT_BINARY_DIR}/amb-get-history ${CMAKE_CURRENT_BINARY_DIR}/amb-listen
    ${CMAKE_CURRENT_BINARY_DIR}/gen-set)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/amb-get.py ${CMAKE_CURRENT_BINARY_DIR}/amb-get @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/amb-set.py ${CMAKE_CURRENT_BINARY_DIR}/amb-set @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/amb-get-history.py ${CMAKE_CURRENT_BINARY_DIR}/amb-get-history @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/amb-listen.py ${CMAKE_CURRENT_BINARY_DIR}/amb-listen @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/gendb.sh ${CMAKE_CURRENT_SOURCE_DIR}/gendb @ONLY)

install (PROGRAMS ${amb_tests} DESTINATION bin)

