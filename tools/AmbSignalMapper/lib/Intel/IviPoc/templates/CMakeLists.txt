cmake_minimum_required(VERSION 2.8)

set( CMAKE_VERBOSE_MAKEFILE on )

# to install plugin into /usr instead of /usr/local
set (CMAKE_INSTALL_PREFIX /usr)

IF(CMAKE_BUILD_TYPE MATCHES DEBUG) 
    message("debug mode") 
ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)
 
IF(CMAKE_BUILD_TYPE MATCHES RELEASE) 
    message("release mode")
    add_definitions(-D_LOGGER_NO_LOG)
ENDIF(CMAKE_BUILD_TYPE MATCHES RELEASE)

include(FindPkgConfig)

find_package(Boost REQUIRED)

pkg_check_modules(glib REQUIRED glib-2.0)
pkg_check_modules(json REQUIRED json)

add_definitions(-std=gnu++0x -fPIC -fPIE)

pkg_check_modules( amb REQUIRED automotive-message-broker )
pkg_check_modules( amb-plugins-common REQUIRED amb-plugins-common )

include_directories(${include_dirs} ${amb-plugins-common_INCLUDE_DIRS} ${amb_INCLUDE_DIRS} ${glib_INCLUDE_DIRS} ${gio_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${json_INCLUDE_DIRS} )

set(ambtmpl_plugin_headers ambtmpl_plugin.h  ambtmpl_cansignals.h ambtmpl_cansignal.h)
set(ambtmpl_plugin_sources ambtmpl_plugin.cpp ambtmpl_cansignal.cpp)

if(ivipoc_tests)
#########################################
# ivipoc_tests START
#########################################

#find CppUTest headers ( CppUTest/CommandLineTestRunner.h ):
find_path(cpputest_INCLUDE_DIR CppUTest/CommandLineTestRunner.h PATHS $ENV{CPPUTEST_HOME}/include DOC "CppUTest headers")

include_directories(${include_dirs} ${cpputest_INCLUDE_DIR})

#find libCppUTestExt.a (and libCppUTest.a) library:
find_library(CppUTest_LIBRARY CppUTest PATHS $ENV{CPPUTEST_HOME}/lib DOC "CppUTest library")
find_library(CppUTestExt_LIBRARY CppUTestExt PATHS $ENV{CPPUTEST_HOME}/lib DOC "CppUTestExt library")

# test definitions
add_definitions(-DUNIT_TESTS -fprofile-arcs -ftest-coverage)

# test link libs
set(link_libraries gcov ${CppUTest_LIBRARY} ${CppUTestExt_LIBRARY})

# test sources
add_subdirectory(test)

#########################################
# ivipoc_tests END
#########################################
endif(ivipoc_tests)

add_library(ambtmpl_plugin MODULE ${ambtmpl_plugin_sources} ${ambtmpl_plugin_headers})
set_target_properties(ambtmpl_plugin PROPERTIES PREFIX "")
target_link_libraries(ambtmpl_plugin ${link_libraries} ${libamb_LIBRARY} ${amb_LIBRARIES} ${amb-plugins-common_LIBRARIES})

install(TARGETS ambtmpl_plugin LIBRARY DESTINATION lib/automotive-message-broker)

############################################################################################################################################
