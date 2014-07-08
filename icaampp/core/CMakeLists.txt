find_package(PkgConfig)
pkg_check_modules(MAGICKPP ImageMagick++)

add_library(core
  aam.cc
  aam.h
  simultaneous_aam.h
  simultaneous_aam.cc
  normalizing_aam.h
  normalizing_aam.cc
  project_out_aam.h
  project_out_aam.cc
  texture.cc
  texture.h
  shape.cc
  shape.h
  imagick.h
  imagick.cc
)

cmake_policy(SET CMP0003 OLD)
if (MAGICKPP_FOUND)
  add_executable(core_test test.cc)
  include_directories(core_test ${MAGICKPP_INCLUDE_DIRS})
  target_link_libraries(core_test newmat core ${MAGICKPP_LIBRARIES})
endif(MAGICKPP_FOUND)
