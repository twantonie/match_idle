#include <fmt/core.h>

#include <opencv2/imgcodecs.hpp>

#include "window_recognition.hpp"

int main() {
  try {
    auto screenshot = cheat::take_screenshot();
    auto test = cheat::resize_to_screen(screenshot, "GemsOfWar");
    cv::imwrite("test.png", test);
  } catch (std::exception &e) {
    fmt::print("Error occurred {}\n", e.what());
  }
}