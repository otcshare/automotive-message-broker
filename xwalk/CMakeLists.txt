if(xwalk_vehicle_extension)

set(vehicle_api_headers vehicle.h vehicle_instance.h vehicle_extension.h common/extension.h picojson.h common/utils.h common/virtual_fs.h
    common/XW_Extension_EntryPoints.h common/XW_Extension.h common/XW_Permissions.h common/XW_Extension_Runtime.h common/XW_Extension_SyncMessage.h)
set(vehicle_api_sources vehicle.cc vehicle_extension.cc vehicle_instance.cc common/extension.cc)

add_library(vehicle_extension MODULE ${vehicle_api_sources})

include_directories(${include_dirs} ${CMAKE_CURRENT_SOURCE_DIR}/)
target_link_libraries(vehicle_extension ${link_libraries} amb)

install(TARGETS vehicle_extension  LIBRARY DESTINATION "${LIB_INSTALL_DIR}/${XWALK_EXTENSION_PATH}")
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/vehicle.html DESTINATION /usr/share/automotive-message-broker/xwalk/examples)
endif(xwalk_vehicle_extension)
