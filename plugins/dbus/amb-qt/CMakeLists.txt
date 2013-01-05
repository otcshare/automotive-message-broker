if(qt_bindings)

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtDBus QtDeclarative REQUIRED)
include(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

set(QT_USE_QTDBUS TRUE)
set(QT_DONT_USE_QTGUI TRUE)
set(ambqt_headers ambqt.h amb-qt_global.h)
set(ambqt_sources ambqt.cpp)
set(ambqtquick_sources ambqtquick.cpp)
set(ambqtquick_headers ambqtquick.h)

QT4_WRAP_CPP(ambqt_headers_moc ${ambqt_headers})
QT4_WRAP_CPP(ambqtquick_headers_moc ${ambqtquick_headers})

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${QT_INCLUDE_DIRS})

add_library(amb-qt SHARED ${ambqt_sources} ${ambqt_headers_moc})
add_library(ambqtquick MODULE ${ambqtquick_sources} ${ambqtquick_headers_moc})

target_link_libraries(amb-qt amb ${QT_LIBRARIES} -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${gio_LIBRARIES})
target_link_libraries(ambqtquick amb-qt ${QT_LIBRARIES} -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${gio_LIBRARIES})

install (TARGETS amb-qt LIBRARY DESTINATION lib)

install (FILES ${ambqt_headers} DESTINATION ${INCLUDE_INSTALL_DIR}/ COMPONENT Devel)
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/qmldir DESTINATION ${LIB_INSTALL_DIR}/${LIB_SUFFIX}/qt4/imports/amb COMPONENT Devel)
install (TARGETS ambqtquick DESTINATION ${LIB_INSTALL_DIR}/${LIB_SUFFIX}/qt4/imports/amb)

endif(qt_bindings)
