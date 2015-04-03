set(amb_tests ${CMAKE_CURRENT_BINARY_DIR}/amb-get ${CMAKE_CURRENT_BINARY_DIR}/amb-set
		${CMAKE_CURRENT_BINARY_DIR}/amb-get-history ${CMAKE_CURRENT_BINARY_DIR}/amb-listen
		${CMAKE_CURRENT_BINARY_DIR}/gen-set)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/amb-get.py ${CMAKE_CURRENT_BINARY_DIR}/amb-get @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/amb-set.py ${CMAKE_CURRENT_BINARY_DIR}/amb-set @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/amb-get-history.py ${CMAKE_CURRENT_BINARY_DIR}/amb-get-history @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/amb-listen.py ${CMAKE_CURRENT_BINARY_DIR}/amb-listen @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/gen-set.py ${CMAKE_CURRENT_BINARY_DIR}/gen-set @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/gendb.sh ${CMAKE_CURRENT_SOURCE_DIR}/gendb @ONLY)

install (PROGRAMS ${amb_tests} DESTINATION bin)

find_package(Qt5Network)
find_package(Qt5Core)

if(Qt5Network_FOUND)
  set(QT_INCLUDE_DIRS ${Qt5Core_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS})
  set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Network_LIBRARIES})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
  add_definitions(${Qt5Core_DEFINITIONS})

  include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${QT_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/plugins/common)

  add_executable(testProtocol testProtocol.cpp testProtocolCommon.h)
  target_link_libraries(testProtocol ${link_libraries} amb -L${CMAKE_CURRENT_BINARY_DIR}/lib amb-plugins-common amb-json-protocol -L${CMAKE_CURRENT_BINARY_DIR}/plugins/common ${QT_LIBRARIES})

  add_executable(testProtocolClient testProtocolClient.cpp testProtocolCommon.h)
  target_link_libraries(testProtocolClient ${link_libraries} amb -L${CMAKE_CURRENT_BINARY_DIR}/lib amb-plugins-common amb-json-protocol -L${CMAKE_CURRENT_BINARY_DIR}/plugins/common ${QT_LIBRARIES})
endif()
