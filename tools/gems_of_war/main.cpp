#include <conio.h>
#include <fmt/core.h>

#include <opencv2/imgcodecs.hpp>
#include <optional>

#include "window_recognition.hpp"

namespace mi = match_idle;

enum class Option { PrintBoard, SaveScreenshot, Continue, Quit };

std::optional<Option> char_to_option(char ch) {
  switch (ch) {
    case 'p':
      return Option::PrintBoard;
    case 's':
      return Option::SaveScreenshot;
    case 'c':
      return Option::Continue;
    case 'q':
      return Option::Quit;
    default:
      return {};
  }
}

Option parse_input() {
  std::optional<Option> option;

  while (!option) {
    fmt::print(
        "Enter character: c (continue), p (print board), s (save sceenshot) or "
        "q (quit)\n");
    char ch = _getch();
    option = char_to_option(ch);
  }

  return *option;
}

size_t largest_match(const mi::PossibleMatch &possible_match) {
  return std::max_element(possible_match.matches.begin(),
                          possible_match.matches.end(),
                          [](const mi::Match &lhs, const mi::Match &rhs) {
                            return lhs.indices.size() < rhs.indices.size();
                          })
      ->indices.size();
}

void print_board(const std::vector<mi::Piece> &board,
                 const mi::GridLayout &grid) {
  fmt::print("{{\n");
  for (size_t r = 0; r < grid.rows; r++) {
    fmt::print("    ");
    for (size_t c = 0; c < grid.cols; c++) {
      size_t index = r * grid.cols + c;
      fmt::print("{{Type::{}}}, ", mi::to_string(board[index].type));
    }
    fmt::print("\n");
  }
  fmt::print("}}");
}

void print_match(const mi::PossibleMatch &possible_match) {
  fmt::print(
      "Location r {} c {} dir {:<5} Largest match: {} Number of matches: {}\n",
      possible_match.pos.row, possible_match.pos.col,
      (possible_match.move_dir == mi::MoveDir::Down ? "down" : "right"),
      largest_match(possible_match), possible_match.matches.size());
}

void print_matches(std::vector<mi::PossibleMatch> matches) {
  // Copy matches so we can mess with it

  std::stable_sort(
      matches.begin(), matches.end(),
      [](const mi::PossibleMatch &lhs, const mi::PossibleMatch &rhs) {
        size_t lhs_size = largest_match(lhs);
        size_t rhs_size = largest_match(rhs);
        if (lhs_size > rhs_size) {
          return true;
        } else if (lhs_size == rhs_size) {
          return lhs.matches.size() > rhs.matches.size();
        } else {
          return false;
        }
      });

  for (auto &possible_match : matches) {
    print_match(possible_match);
  }
}

int main() {
  static constexpr mi::GridLayout grid{8, 8};

  bool going{true};
  while (going) {
    auto screenshot = cheat::take_screenshot();
    auto window = cheat::resize_to_screen(screenshot, "GemsOfWar");

    auto board = cheat::read_board(window);
    auto matches =
        mi::find_possible_matches(board, grid, mi::RemoveType::Upgrade);
    print_matches(matches);

    bool parsing{true};
    while (parsing) {
      auto option = parse_input();

      switch (option) {
        case Option::PrintBoard:
          print_board(board, grid);
          break;
        case Option::SaveScreenshot:
          cv::imwrite("screenshot.png", window);
          break;

        case Option::Quit:
          going = false;
          // fall-through
        case Option::Continue:
          parsing = false;
          break;
      }
    }
  }
}