add_executable(play_gems main.cpp)
target_link_libraries(play_gems PRIVATE fmt::fmt lib)

add_executable(take_screenshot take_screenshot.cpp)
target_link_libraries(take_screenshot PRIVATE fmt::fmt lib)
