project(nobdy2)
cmake_minimum_required(VERSION 2.8)

include(FindPkgConfig)

set (LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
set (LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/${CMAKE_LIBRARY_ARCHITECTURE}" CACHE STRING "Directory where lib will install")
set (INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "The directory the headers are installed in")

set(PROJECT_NAME "nobdy2")
set(PROJECT_VERSION "0.0.1")

option(use_qtcore "Use QCoreApplication mainloop " OFF)
option(nobdy_compat "Build nobdy compatibility plugin" OFF)

find_library(libtool_LIBRARY ltdl DOC "Libtool libraries")
find_path(libtool_INCLUDE_DIR ltdl.h DOC "Libtool headers")
find_package(Boost REQUIRED)
pkg_check_modules(glib REQUIRED glib-2.0)
pkg_check_modules(gio REQUIRED gio-2.0)

set(include_dirs ${libtool_INCLUDE_DIR} ${glib_INCLUDE_DIRS} ${gio_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} "lib")
set(link_libraries -lnobdy2 ${glib_LIBRARIES} ${gio_LIBRARIES} -L${CMAKE_CURRENT_BINARY_DIR})

add_executable(nobdy2 main.cpp)

add_definitions(-std=c++0x)
add_definitions(-DDBusServiceName="org.automotive")

if(use_qtcore)

	find_package( Qt4 REQUIRED )
	set( QT_DONT_USE_QTGUI TRUE )
	include(${QT_USE_FILE})
	set(include_dirs ${include_dirs} ${qt_INCLUDE_DIRS})
	set(link_libraries ${link_libraries} ${qt_LIBRARIES})

	add_definitions(-DUSE_QT_CORE)

endif(use_qtcore)

include_directories(${include_dirs})
target_link_libraries(nobdy2 ${link_libraries})

add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

add_subdirectory(lib)
add_subdirectory(plugins)
