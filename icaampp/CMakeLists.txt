cmake_minimum_required(VERSION 2.4)
project(icaampp)

add_definitions(-g)

add_subdirectory(ext/newmat)

include_directories(ext/)
add_subdirectory(core)
add_subdirectory(gui)


