set(amb_sources abstractpropertytype.cpp abstractroutingengine.cpp listplusplus.cpp abstractsink.cpp vehicleproperty.cpp abstractsource.cpp debugout.cpp timestamp.cpp uuidhelper.cpp mappropertytype.hpp propertyinfo.hpp superptr.hpp asyncqueue.hpp ambpluginimpl.cpp ambplugin.h picojson.h jsonhelper.cpp)
set(amb_headers_install abstractpropertytype.h nullptr.h abstractroutingengine.h listplusplus.h abstractsink.h vehicleproperty.h debugout.h abstractsource.h timestamp.h uuidhelper.h mappropertytype.hpp propertyinfo.hpp superptr.hpp asyncqueue.hpp ambplugin.h ambpluginimpl.h picojson.h jsonhelper.h)

add_library(amb SHARED ${amb_sources})

find_library(uuid_LIBRARY uuid DOC "Uuid libraries")
find_path(uuid_INCLUDE_DIR uuid/uuid.h DOC "Libtool headers")

if(uuid_LIBRARY)
	 message(STATUS "uuid found")
else(uuid_LIBRARY)
	 message(FATAL_ERROR "uuid missing.  please install uuid-dev")
endif(uuid_LIBRARY)

include_directories( ${include_dirs} ${uuid_INCLUDE_DIR})

target_link_libraries(amb ${libtool_LIBRARY} ${glib_LIBRARIES} ${gio_LIBRARIES} ${uuid_LIBRARY} ${json_LIBRARIES})
set_target_properties(amb PROPERTIES SOVERSION ${PROJECT_VERSION})

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/automotive-message-broker.pc.in ${CMAKE_CURRENT_BINARY_DIR}/automotive-message-broker.pc @ONLY)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/automotive-message-broker.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)

install (FILES ${amb_headers_install} DESTINATION ${INCLUDE_INSTALL_DIR}/amb COMPONENT Devel)

install (TARGETS amb LIBRARY DESTINATION ${LIB_INSTALL_DIR})


