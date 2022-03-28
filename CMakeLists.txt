cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(match_idle LANGUAGES C CXX)

option(COMPILE_TESTS "Compile tests" ON)

set(CMAKE_CXX_STANDARD 17)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}" "${CMAKE_SOURCE_DIR}/cmake")

include(settings)
include(warnings)
include(analyzers)

find_package(centurion REQUIRED)
find_package(spdlog REQUIRED)
find_package(PhysFS REQUIRED)
find_package(Boost REQUIRED)

file(GLOB ZIP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/assets/*")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/bin/assets.zip"
  COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${CMAKE_BINARY_DIR}/bin/assets.zip"
          --format=zip "${CMAKE_SOURCE_DIR}/assets"
  DEPENDS ${ZIP_FILES}
  VERBATIM USES_TERMINAL)

add_library(lib STATIC src/match_window.cpp src/assets.cpp)
target_link_libraries(lib PUBLIC centurion::centurion fmt::fmt physfs::physfs
                                 Boost::Boost)
target_include_directories(lib PUBLIC src)

add_executable(match_idle src/main.cpp)
target_link_libraries(match_idle PRIVATE lib)

configure_file(resources/index.html "${CMAKE_BINARY_DIR}/bin/index.html"
               COPYONLY)

if(COMPILE_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
