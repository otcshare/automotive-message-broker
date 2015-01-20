
find_program(rpmbuild rpmbuild /usr/bin)
if(rpmbuild)
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/automotive-message-broker.spec.in ${CMAKE_SOURCE_DIR}/packaging/automotive-message-broker.spec @ONLY)
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/config.tizen.in ${CMAKE_SOURCE_DIR}/packaging/config.tizen @ONLY)
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/automotive-message-broker.changes.in ${CMAKE_SOURCE_DIR}/packaging/automotive-message-broker.changes @ONLY)
endif(rpmbuild)

find_program(debuild debuild /usr/bin)
if(debuild)
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/debian/control.debian.in ${CMAKE_SOURCE_DIR}/debian/control @ONLY)
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/debian/libamb.debian.install ${CMAKE_SOURCE_DIR}/debian/libamb.install @ONLY)
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/debian/libamb-dev.debian.install ${CMAKE_SOURCE_DIR}/debian/libamb-dev.install @ONLY)
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/debian/automotive-message-broker.debian.install ${CMAKE_SOURCE_DIR}/debian/automotive-message-broker.install @ONLY)
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/debian/automotive-message-broker-plugins.debian.install ${CMAKE_SOURCE_DIR}/debian/automotive-message-broker-plugins.install @ONLY)
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/debian/changelog ${CMAKE_SOURCE_DIR}/debian/changelog @ONLY)
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/debian/rules ${CMAKE_SOURCE_DIR}/debian/rules @ONLY)
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/debian/automotive-message-broker-plugins-bluemonkey.debian.install
				  ${CMAKE_SOURCE_DIR}/debian/automotive-message-broker-plugins-bluemonkey.install @ONLY)
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/debian/automotive-message-broker-plugins-bluetooth.debian.install
				  ${CMAKE_SOURCE_DIR}/debian/automotive-message-broker-plugins-bluetooth.install @ONLY)
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/debian/automotive-message-broker-plugins-qtmainloop.debian.install
				  ${CMAKE_SOURCE_DIR}/debian/automotive-message-broker-plugins-qtmainloop.install @ONLY)
endif(debuild)
