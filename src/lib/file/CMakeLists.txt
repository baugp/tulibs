remake_find_package(ZLIB)
remake_find_package(BZip2)

remake_include(
  ${ZLIB_INCLUDE_DIRS}
  ${BZIP2_INCLUDE_DIRS}
)
remake_add_library(
  file
  LINK ${ZLIB_LIBRARY} ${BZIP2_LIBRARIES}
)
remake_add_headers(INSTALL file)
