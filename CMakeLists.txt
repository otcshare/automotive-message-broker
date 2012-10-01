project(automotive-message-broker)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE, Debug)

include(FindPkgConfig)

set(PROJECT_NAME "automotive-message-broker")
set(PROJECT_VERSION "0.4.0")

add_definitions(-DPROJECT_VERSION="${PROJECT_VERSION}")
add_definitions(-DPROJECT_NAME="${PROJECT_NAME}")

set (LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
set (LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/${CMAKE_LIBRARY_ARCHITECTURE}" CACHE STRING "Directory where lib will install")
set (INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "The directory the headers are installed in")
set (DOC_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/doc/${PROJECT_NAME}" CACHE PATH "The directory the docs will be installed to")

option(use_qtcore "Use QCoreApplication mainloop " OFF)

find_library(libtool_LIBRARY ltdl DOC "Libtool libraries")
find_path(libtool_INCLUDE_DIR ltdl.h DOC "Libtool headers")
find_package(Boost REQUIRED)

pkg_check_modules(glib REQUIRED glib-2.0)
pkg_check_modules(json REQUIRED json-glib-1.0)

add_definitions(-std=c++0x)
add_definitions(-DDBusServiceName="org.automotive.message.broker")

set(include_dirs ${libtool_INCLUDE_DIR} ${glib_INCLUDE_DIRS} ${gio_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${json_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(link_libraries -lamb ${libtool_LIBRARY} ${glib_LIBRARIES} ${json_LIBRARIES} -L${CMAKE_CURRENT_BINARY_DIR}/lib)

if(use_qtcore)

	find_package( Qt4 REQUIRED )
	set( QT_DONT_USE_QTGUI TRUE )
	include(${QT_USE_FILE})
	set(include_dirs ${include_dirs} ${qt_INCLUDE_DIRS})
	set(link_libraries ${link_libraries} ${qt_LIBRARIES})

	add_definitions(-DUSE_QT_CORE)

endif(use_qtcore)

add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

set(ARCHIVE_NAME ${CMAKE_PROJECT_NAME}-${PROJECT_VERSION})
add_custom_target(dist COMMAND git archive --prefix=${ARCHIVE_NAME}/ HEAD | bzip2 > ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.bz2 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_subdirectory(lib)
add_subdirectory(ambd)
add_subdirectory(plugins)
add_subdirectory(docs)
add_subdirectory(tests)
