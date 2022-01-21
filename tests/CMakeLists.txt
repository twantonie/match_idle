find_package(Catch2 REQUIRED)
include(Catch)

add_executable(tests match_window.cpp)
target_link_libraries(tests PRIVATE lib Catch2::Catch2WithMain)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

catch_discover_tests(tests)
