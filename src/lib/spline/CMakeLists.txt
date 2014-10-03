remake_find_package(gsl CONFIG)

remake_add_library(
  spline
  LINK string file error ${GSL_LIBRARIES}
)
remake_add_headers(INSTALL spline)
