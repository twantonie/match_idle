add_executable(treasure_hunt treasure_hunt.cpp)
target_link_libraries(treasure_hunt PRIVATE fmt::fmt lib)

add_executable(take_screenshot take_screenshot.cpp)
target_link_libraries(take_screenshot PRIVATE fmt::fmt lib)
