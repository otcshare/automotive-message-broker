if(xwalk_vehicle_extension)

pkg_check_modules(gio REQUIRED gio-2.0)

set(vehicle_api_headers vehicle.h vehicle_instance.h vehicle_extension.h common/extension.h picojson.h common/utils.h common/virtual_fs.h
    common/XW_Extension_EntryPoints.h common/XW_Extension.h common/XW_Permissions.h common/XW_Extension_Runtime.h common/XW_Extension_SyncMessage.h)
set(vehicle_api_sources vehicle.cc vehicle_extension.cc vehicle_instance.cc common/extension.cc vehicle_api.cc)

add_library(vehicle_extension MODULE ${vehicle_api_sources})

include_directories(${include_dirs} ${CMAKE_CURRENT_SOURCE_DIR}/)
target_link_libraries(vehicle_extension ${link_libraries} amb ${gio_LIBRARIES} -L${CMAKE_CURRENT_BINARY_DIR}/lib)

add_custom_command(OUTPUT genjs2cc COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/generate_api.py ${CMAKE_CURRENT_SOURCE_DIR}/vehicle_api.js kSource_vehicle_api ${CMAKE_CURRENT_SOURCE_DIR}/vehicle_api.cc )
add_custom_target(js3cc DEPENDS genjs2cc)
add_dependencies(vehicle_extension js3cc)

install(TARGETS vehicle_extension  LIBRARY DESTINATION "${LIB_INSTALL_DIR}/${XWALK_EXTENSION_PATH}")
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/vehicle.html DESTINATION /usr/share/automotive-message-broker/xwalk/examples)
endif(xwalk_vehicle_extension)
