remake_find_package(libusb-1.0 CONFIG)
remake_find_package(libudev CONFIG)

remake_include(
  ${LIBUSB_1_0_INCLUDE_DIRS}
  ${LIBUDEV_INCLUDEDIR}
)
remake_add_library(
  usb
  LINK ${LIBUSB_1_0_LIBRARIES} ${LIBUDEV_LIBRARIES}
)
remake_add_headers()
