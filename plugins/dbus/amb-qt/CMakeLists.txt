if(qt_bindings)

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtDBus REQUIRED)
include(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

set(QT_USE_QTDBUS TRUE)
set(QT_DONT_USE_QTGUI TRUE)
set(ambqt_headers ambqt.h amb-qt_global.h)
set(ambqt_sources ambqt.cpp)

QT4_WRAP_CPP(ambqt_headers_moc ${ambqt_headers})

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${QT_INCLUDE_DIRS})

add_library(amb-qt MODULE ${ambqt_sources} ${ambqt_headers_moc})

target_link_libraries(amb-qt amb ${QT_LIBRARIES} -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${gio_LIBRARIES})

install (TARGETS amb-qt LIBRARY DESTINATION lib)

install (FILES ${ambqt_headers} DESTINATION ${INCLUDE_INSTALL_DIR}/ COMPONENT Devel)

endif(qt_bindings)
