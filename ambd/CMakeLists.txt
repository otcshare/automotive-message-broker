set(ambd_headers core.h imainloop.h pluginloader.h glibmainloop.h)
set(ambd_sources core.cpp main.cpp pluginloader.cpp glibmainloop.cpp)

if(qtmainloop)
    set(qtmainloopplugin_headers qtmainloop.h)
    set(qtmainloopplugin_sources qtmainloop.cpp)

    add_library(qtmainloopplugin MODULE ${qtmainloopplugin_sources})
    set_target_properties(qtmainloopplugin PROPERTIES PREFIX "")
    target_link_libraries(qtmainloopplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})

    install(TARGETS qtmainloopplugin LIBRARY DESTINATION lib/automotive-message-broker)
endif(qtmainloop)

add_executable(ambd ${ambd_sources})

include_directories(${include_dirs} )
target_link_libraries(ambd ${link_libraries} amb)

install (TARGETS ambd RUNTIME DESTINATION bin)
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/config DESTINATION /etc/ambd)
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/ambd.service DESTINATION /usr/lib/systemd/system)

