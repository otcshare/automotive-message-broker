
set(amb_sources abstractroutingengine.cpp listplusplus.cpp abstractsink.cpp vehicleproperty.cpp abstractsource.cpp debugout.cpp)
set(amb_headers_install vehicleproperty.h debugout.h abstractsource.h)
include_directories( ${include_dirs} )
add_library(amb SHARED ${amb_sources})

target_link_libraries(amb ${libtool_LIBRARY} ${glib_LIBRARIES} ${gio_LIBRARIES})

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/automotive-message-broker.pc.in ${CMAKE_CURRENT_BINARY_DIR}/automotive-message-broker.pc @ONLY)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/automotive-message-broker.pc DESTINATION ${LIB_INSTALL_DIR}/${LIB_SUFFIX}/pkgconfig)
install (FILES ${amb_headers_install} DESTINATION ${INCLUDE_INSTALL_DIR}/amb COMPONENT Devel)
install (TARGETS amb LIBRARY DESTINATION ${LIB_INSTALL_DIR} RUNTIME DESTINATION bin ARCHIVE DESTINATION ${LIB_INSTALL_DIR})


