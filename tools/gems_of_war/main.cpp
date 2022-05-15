#include <fmt/core.h>

#include <opencv2/imgcodecs.hpp>

#include "window_recognition.hpp"

int main() {
  try {
    auto screenshot = cheat::take_screenshot();
    auto window = cheat::resize_to_screen(screenshot, "GemsOfWar");
    cv::imwrite("window.png", window);

    auto board = cheat::read_board(window);

  } catch (std::exception &e) {
    fmt::print("Error occurred {}\n", e.what());
  }
}