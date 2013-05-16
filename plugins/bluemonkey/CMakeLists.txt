if(bluemonkey_plugin)


FIND_PACKAGE(Qt4 COMPONENTS QtCore QtNetwork QtScript REQUIRED)
include(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

set(QT_USE_QSCRIPT TRUE)
set(QT_USE_NETWORK TRUE)
set(QT_DONT_USE_QTGUI TRUE)

set(communi_INCLUDE_DIRS /usr/include/qt4/Communi)
set(communi_LIBRARIES -lCommuni)
add_definitions(-DCOMMUNI_SHARED)

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${communi_INCLUDE_DIRS})

set(bluemonkeyplugin_headers bluemonkey.h irccoms.h)
set(bluemonkeyplugin_sources bluemonkey.cpp irccoms.cpp)

add_library(bluemonkeyplugin MODULE ${bluemonkeyplugin_sources})
set_target_properties(bluemonkeyplugin PROPERTIES PREFIX "")
target_link_libraries(bluemonkeyplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${QT_LIBRARIES} ${communi_LIBRARIES})

install(TARGETS bluemonkeyplugin LIBRARY DESTINATION lib/automotive-message-broker)

endif(bluemonkey_plugin)
