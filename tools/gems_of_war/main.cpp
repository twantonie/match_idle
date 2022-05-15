#include <fmt/core.h>

#include <opencv2/imgcodecs.hpp>

#include "window_recognition.hpp"

namespace mi = match_idle;

int main() {
  static constexpr mi::GridLayout grid{8, 8};

  auto screenshot = cheat::take_screenshot();
  auto window = cheat::resize_to_screen(screenshot, "GemsOfWar");

  try {
    auto board = cheat::read_board(window);
    auto matches = mi::find_possible_matches(board, grid);

    for (auto &possible_match : matches) {
      fmt::print(
          "Location r {} c {} dir {} Number of matches: {}\n",
          possible_match.pos.row, possible_match.pos.col,
          (possible_match.move_dir == mi::MoveDir::Down ? "down" : "right"),
          possible_match.matches.size());
      for (auto &match : possible_match.matches) {
        fmt::print("\tsize {}\n", match.indices.size());
      }
    }
  } catch (std::exception &e) {
    fmt::print("Error occurred {}\n", e.what());
    cv::imwrite("window.png", window);
  }
}