
find_package(PkgConfig)
pkg_check_modules(GTKMM REQUIRED gtkmm-2.4>=2.10)
pkg_check_modules(CAIROMM REQUIRED cairomm-1.0)
pkg_check_modules(GTHREAD REQUIRED gthread-2.0)

link_directories(../core ${GTKMM_LIBRARY_DIRS} ${CAIROMM_LIBRARY_DIRS} ${GTHREAD_LIBRARY_DIRS})
include_directories(../core ${GTKMM_INCLUDE_DIRS} ${CAIROMM_INCLUDE_DIRS} ${GTHREAD_INCLUDE_DIRS} ${MAGICKPP_INCLUDE_DIRS})

# avoid warning with CMake 2.6 (should find a way to fix the cause of the warning instead)
cmake_policy(VERSION 2.4)
add_executable(test
  main.cc
  testwindow.cc
  testwindow.h
  cairoarea.cc
  cairoarea.h
  )
target_link_libraries(test core newmat ${GTKMM_LIBRARIES} ${CAIROMM_LIBRARIES} ${MAGICKPP_LIBRARIES} ${GTHREAD_LIBRARIES})  


