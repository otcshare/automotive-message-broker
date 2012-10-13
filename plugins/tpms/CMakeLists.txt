include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs})

pkg_check_modules(libusb REQUIRED libusb-1.0)
set(link_libraries ${link_libraries} ${libusb_LIBRARIES})
include_directories(${libusb_INCLUDE_DIRS})
#check_include_files(libusb.h HAVE_USB)
#set(CMAKE_CXX_FLAGS "-g -I/usr/include/libusb-1.0/ -lusb-1.0")

set(tpmsplugin_headers tpmsplugin.h)
set(tpmsplugin_sources tpmsplugin.cpp)

add_library(tpmsplugin MODULE ${tpmsplugin_sources})
set_target_properties(tpmsplugin PROPERTIES PREFIX "")
target_link_libraries(tpmsplugin amb -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})
