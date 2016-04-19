project(automotive-message-broker)
cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_BUILD_TYPE, Debug)

include(FindPkgConfig)
include(CMakeDependentOption)
include(GNUInstallDirs)

set(PROJECT_NAME "automotive-message-broker")
set(PROJECT_PRETTY_NAME "Automotive Message Broker")
set(PROJECT_SERIES "0.15")
set(PROJECT_MAJOR_VERSION "0.14")
set(PROJECT_MINOR_VERSION "802")
set(PROJECT_VERSION "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}")
set(PROJECT_CODENAME "")
set(PROJECT_QUALITY "alpha")

add_definitions(-DPROJECT_VERSION="${PROJECT_VERSION}")
add_definitions(-DPROJECT_NAME="${PROJECT_PRETTY_NAME}")
add_definitions(-DPROJECT_CODENAME="${PROJECT_CODENAME}")
add_definitions(-DPROJECT_QUALITY="${PROJECT_QUALITY}")
add_definitions(-DPROJECT_SERIES="${PROJECT_SERIES}")

set (LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
set (LIB_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}" )
set (PLUGIN_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}")
set (INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
set (DOC_INSTALL_DIR "${CMAKE_INSTALL_DOCDIR}/${PROJECT_SERIES}")
set (PLUGIN_SEGMENT_INSTALL_PATH "/${CMAKE_INSTALL_SYSCONFDIR}/ambd/plugins.d")

option(qtmainloop "Use QCoreApplication mainloop " OFF)
option(websocket_plugin "websocket source and sink plugins" OFF)
option(obd2_plugin "OBD-II plugin" OFF)
option(database_plugin "Database plugins" OFF)
option(qt_bindings "AMB Qt DBus bindings" OFF)
option(opencvlux_plugin "OpenCV Lux plugin" OFF)
option(murphy_plugin "murphy policy framework plugin" OFF)
option(test_plugin "Test Plugin" ON)
option(bluemonkey_plugin "bluemonkey plugin" OFF)
option(gpsnmea_plugin "gps NMEA location plugin" OFF)
option(chrony_plugin "chrony plugin" OFF)
option(openxc_plugin "OpenXC plugin" OFF)
option(bluetooth_plugin "bluetooth plugin" OFF)
option(cansim_plugin "Can simulator plugin" OFF)
option(cangen_plugin "Can generator plugin" OFF)
option(enable_icecc "Enable icecc checking, for distributed compilation" ON)
option(enable_docs "enable Doxygen doc generation" OFF)
option(usebluez5 "use bluez 5 API" OFF)
option(xwalk_vehicle_extension "Crosswalk vehicle extension" OFF)
set(XWALK_EXTENSION_PATH "/automotive-message-broker/xwalk" CACHE PATH "directory the xwalk extension will be installed to")
set(QMAKE_INSTALL_PATH "/usr/bin/qmake" CACHE PATH "qmake executable path")

#turn on -fpic/-fpie:
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpie -pie -std=c++14")

if (enable_icecc)
	include (CMakeForceCompiler)
	find_program(ICECC /usr/lib/icecc/bin/g++)
		if (ICECC)
			message(STATUS "icecc will be used for distributed compiling")
			cmake_force_cxx_compiler(${ICECC} icecc)
		else(ICECC)
			message(STATUS "Not using icecc")
		endif(ICECC)
endif(enable_icecc)

find_package(Boost REQUIRED)


pkg_check_modules(glib REQUIRED glib-2.0 gobject-2.0)

add_definitions(-DDBusServiceName="org.automotive.message.broker")

set(include_dirs ${libtool_INCLUDE_DIR} ${glib_INCLUDE_DIRS} ${gio_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${json_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(link_libraries -lamb ${glib_LIBRARIES} ${json_LIBRARIES} -L${CMAKE_CURRENT_BINARY_DIR}/lib)

if(usebluez5)
	add_definitions(-DUSE_BLUEZ5)

endif(usebluez5)

include(CheckCSourceCompiles)
check_c_source_compiles("
        #include <linux/can/bcm.h>
        int main() { struct bcm_timeval t; return 0;}" have_bcm_timeval)

if(have_bcm_timeval)
        add_definitions(-DHAVE_BCM_TIMEVAL)
endif(have_bcm_timeval)

set(ARCHIVE_NAME ${CMAKE_PROJECT_NAME}-${PROJECT_VERSION})
add_custom_target(dist COMMAND git archive --prefix=${ARCHIVE_NAME}/ HEAD | bzip2 > ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.bz2 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RELEASE.in.md" "${CMAKE_CURRENT_SOURCE_DIR}/RELEASE.md" @ONLY)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/TODO" "${CMAKE_CURRENT_SOURCE_DIR}/TODO.md" @ONLY)

# packaging stuff:
# Common things to every type of package
SET(CPACK_PACKAGE_DESCRIPTION "daemon that provides access vehicle data")
SET(CPACK_PACKAGE_NAME "automotive-message-broker")
SET(CPACK_PACKAGE_EXECUTABLES "ambd")
SET(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
SET(CPACK_STRIP_FILES true)
SET(CPACK_PACKAGE_CONTACT "tripzero.kev@gmail.com")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")

# Always build .tar.gz and .sh files
SET(CPACK_GENERATOR "STGZ;TGZ;TBZ2")

add_subdirectory(lib)
add_subdirectory(plugins)
add_subdirectory(ambd)
add_subdirectory(docs)
add_subdirectory(tests)
add_subdirectory(tools)
add_subdirectory(examples)
add_subdirectory(xwalk)
add_subdirectory(packaging.in)


