remake_find_package(libftdi CONFIG)
remake_find_package(libudev CONFIG)

remake_include(
  ${LIBFTDI_INCLUDEDIR}
  ${LIBUDEV_INCLUDEDIR}
)
remake_add_library(
  tulibs-ftdi
  PREFIX OFF
  LINK timer error ${LIBFTDI_LIBRARIES} ${LIBUDEV_LIBRARIES}
  NO_INCLUDE
)
remake_add_headers(INSTALL ftdi)
