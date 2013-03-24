cmake_minimum_required(VERSION 2.6)

include(ReMake)

remake_project(
  Tulibs
  VERSION 0.1
  RELEASE alpha
  SUMMARY "A collection of C-based Tool and Utility LIBrarieS."
  AUTHOR "Ralf Kaestner"
  CONTACT "ralf.kaestner@gmail.com"
  HOME "http://aslforge.ethz.ch/projects/tulibs"
  LICENSE "GNU Lesser General Public License (LGPL)"
  INSTALL /usr
)

remake_doc(
  html
)

remake_pack_deb(
  DEPENDS libgsl0ldbl
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
  DEPENDS remake libgsl0-dev doxygen
  PASS CMAKE_BUILD_TYPE TULIBS_SUBVERSION_REVISION
)
remake_distribute_deb(
  DISTRIBUTION precise
  SECTION libs
  UPLOAD ppa:kralf/asl
  DEPENDS remake libgsl0-dev doxygen
  PASS CMAKE_BUILD_TYPE TULIBS_SUBVERSION_REVISION
)
