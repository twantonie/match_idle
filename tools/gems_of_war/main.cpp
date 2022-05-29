#include <fmt/core.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <optional>

#include "window_recognition.hpp"

namespace mi = match_idle;

enum class Option { PrintBoard, SaveScreenshot, Continue, Quit, Apply };

// TODO: Add option to automatically apply best match

constexpr cheat::Key print_board_k{1, 0x50};
constexpr cheat::Key save_screenshot_k{2, 0x53};
constexpr cheat::Key continue_k{3, 0x43};
constexpr cheat::Key quit_k{4, 0x51};
constexpr cheat::Key apply_k{5, 0x41};

std::optional<Option> id_to_option(int id) {
  switch (id) {
    case print_board_k.id:
      return Option::PrintBoard;
    case save_screenshot_k.id:
      return Option::SaveScreenshot;
    case continue_k.id:
      return Option::Continue;
    case quit_k.id:
      return Option::Quit;
    case apply_k.id:
      return Option::Apply;
    default:
      return {};
  }
}

void register_keys() {
  cheat::register_key(print_board_k);
  cheat::register_key(save_screenshot_k);
  cheat::register_key(continue_k);
  cheat::register_key(quit_k);
  cheat::register_key(apply_k);
}

Option parse_input() {
  std::optional<Option> option;

  while (!option) {
    fmt::print(
        "Press key + ctrl: c (continue), p (print board), s (save sceenshot) "
        ", q (quit) or a (apply)\n");
    int id = cheat::get_hotkey_id();
    if (id != 0) {
      option = id_to_option(id);
    }
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
          if (lhs.matches.size() > rhs.matches.size()) {
            return true;
          } else if (lhs.matches.size() == rhs.matches.size()) {
            return lhs.pos.row > rhs.pos.row;
          } else {
            return false;
          }
        } else {
          return false;
        }
      });

  for (auto &possible_match : matches) {
    print_match(possible_match);
  }
}

int main() {
  static constexpr char window_name_k[] = "GemsOfWar";
  static constexpr mi::GridLayout grid{8, 8};

  register_keys();
  auto screen_pos = cheat::screen_position(window_name_k);

  bool going{true};
  while (going) {
    auto screenshot = cheat::take_screenshot();
    cv::Mat window;
    cv::cvtColor(screenshot(screen_pos), window, cv::COLOR_BGRA2BGR);

    auto board = cheat::read_board(window);
    auto matches =
        mi::find_possible_matches(board, grid, mi::RemoveType::Upgrade);
    print_matches(matches);

    bool parsing{true};
    while (parsing) {
      auto option = parse_input();

      switch (option) {
        case Option::PrintBoard:
          mi::print_board(board, grid);
          break;
        case Option::SaveScreenshot:
          cv::imwrite("screenshot.png", window);
          break;
        case Option::Apply: {
          auto &match = matches.front();
          cheat::move_piece(match.move_dir, match.pos, screen_pos);
          break;
        }
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