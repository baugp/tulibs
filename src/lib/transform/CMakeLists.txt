remake_find_library(gsl gsl_blas.h)
remake_find_library(gslcblas gsl_blas.h PACKAGE gsl)

remake_add_library(
  transform
  LINK ${GSL_LIBRARY} ${GSLCBLAS_LIBRARY}
)
remake_add_headers()
