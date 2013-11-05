remake_find_package(gsl CONFIG)

remake_add_library(
  transform
  LINK ${GSL_LIBRARIES}
)
remake_add_headers(INSTALL transform)
