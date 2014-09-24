cmake_minimum_required(VERSION 2.6)

include(ReMake)

remake_project(
  Tulibs
  VERSION 0.5
  RELEASE alpha
  SUMMARY "A collection of C-based Tool and Utility LIBrarieS."
  AUTHOR "Ralf Kaestner"
  CONTACT "ralf.kaestner@gmail.com"
  HOME "http://github.com/kralf/tulibs"
  LICENSE "GNU Lesser General Public License (LGPL)"
  INSTALL /usr
)

remake_doc(
  man INSTALL share
  html
)

remake_pack_deb(
  DEPENDS libudev[0-1][:a-z]* libusb-1.0-0[:a-z]* libftdi1 libgsl0ldbl
    zlib1g libbz2-1.0
)
remake_pack_deb(
  COMPONENT utils
  DESCRIPTION "utilities"
  DEPENDS tulibs
)
remake_pack_deb(
  COMPONENT dev
  DESCRIPTION "development headers"
  DEPENDS tulibs
)
remake_pack_deb(
  ARCH all
  COMPONENT doc
  DESCRIPTION "documentation"
)

remake_distribute_deb(
  DISTRIBUTION lucid
  SECTION libs
  UPLOAD ppa:kralf/asl
  DEPENDS libudev-dev libusb-1.0-0-dev libftdi-dev libgsl0-dev
    zlib1g-dev libbz2-dev remake pkg-config doxygen
  PASS CMAKE_BUILD_TYPE TULIBS_GIT_REVISION
)
remake_distribute_deb(
  DISTRIBUTION precise
  SECTION libs
  UPLOAD ppa:kralf/asl
  DEPENDS libudev-dev libusb-1.0-0-dev libftdi-dev libgsl0-dev
    zlib1g-dev libbz2-dev remake pkg-config doxygen
  PASS CMAKE_BUILD_TYPE TULIBS_GIT_REVISION
)
remake_distribute_deb(
  DISTRIBUTION trusty
  SECTION libs
  UPLOAD ppa:kralf/asl
  DEPENDS libudev-dev libusb-1.0-0-dev libftdi-dev libgsl0-dev
    zlib1g-dev libbz2-dev remake pkg-config doxygen
  PASS CMAKE_BUILD_TYPE TULIBS_GIT_REVISION
)
