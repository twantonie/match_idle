#include <fmt/core.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <window_recognition.hpp>

int main(int argc, char** argv) {
  if (argc != 2) {
    fmt::print("Expected take_screenshot <screenshot_name>\n");
    return -1;
  }

  static constexpr char window_name_k[] = "GemsOfWar";

  auto screen_pos = cheat::screen_position(window_name_k);
  auto screenshot = cheat::take_screenshot();

  cv::Mat window;
  cv::cvtColor(screenshot(screen_pos), window, cv::COLOR_BGRA2BGR);

  cv::imwrite(argv[1], window);
}