cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(match_idle LANGUAGES C CXX)

option(COMPILE_TESTS "Compile tests" ON)

set(CMAKE_CXX_STANDARD 17)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}" "${CMAKE_SOURCE_DIR}/cmake")

include(settings)
include(warnings)
include(analyzers)

find_package(centurion REQUIRED)

add_library(lib STATIC src/match_window.cpp)
target_link_libraries(lib PUBLIC centurion::centurion)
target_include_directories(lib PUBLIC src)

add_executable(match_idle src/main.cpp)
target_link_libraries(match_idle PRIVATE lib)

configure_file(src/index.html "${CMAKE_BINARY_DIR}/bin/index.html" COPYONLY)

if(COMPILE_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
