configure_file (${CMAKE_CURRENT_SOURCE_DIR}/genmapping.py ${CMAKE_CURRENT_SOURCE_DIR}/genmapping @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/ambctl.py ${CMAKE_CURRENT_BINARY_DIR}/ambctl @ONLY)

install (PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/ambctl DESTINATION bin)
