remake_find_package(libusb-1.0 CONFIG)
remake_find_package(libudev CONFIG)

remake_include(
  ${LIBUDEV_INCLUDEDIR}
  ${LIBUSB_1_0_INCLUDE_DIRS}
)
remake_add_library(
  usb
  LINK ${LIBUDEV_LIBRARIES} ${LIBUSB_1_0_LIBRARIES}
)
remake_add_headers()
