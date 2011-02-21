remake_find_package(Threads)
if(NOT ${CMAKE_USE_PTHREADS_INIT})
  message(FATAL_ERROR "Missing POSIX thread support!")
endif(NOT ${CMAKE_USE_PTHREADS_INIT})
remake_find_library(rt time.h)

remake_include(../timer)
remake_add_library(
  thread
  LINK timer ${CMAKE_THREAD_LIBS_INIT} ${RT_LIBRARY}
)
remake_add_headers()
