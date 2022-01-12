cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(match_idle LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}" "${CMAKE_SOURCE_DIR}/cmake")

include(settings)
include(warnings)

find_package(centurion REQUIRED)

add_library(lib STATIC src/match_window.cpp)
target_link_libraries(lib PUBLIC centurion::centurion)

add_executable(match_idle src/main.cpp)
target_link_libraries(match_idle PRIVATE lib)
