#include <fmt/chrono.h>
#include <fmt/core.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <optional>

#include "window_recognition.hpp"

namespace mi = match_idle;

enum class Option { PrintBoard, SaveScreenshot, Continue, Quit, Apply };

constexpr cheat::Key quit_k{4, 0x51};

std::optional<Option> id_to_option(int id) {
  switch (id) {
    case quit_k.id:
      return Option::Quit;
    default:
      return {};
  }
}

void register_keys() { cheat::register_key(quit_k); }

Option parse_input() {
  std::optional<Option> option;

  while (!option) {
    fmt::print("Press ctrl key + q to quit\n");
    int id = cheat::get_hotkey_id();
    if (id != 0) {
      option = id_to_option(id);
    }
  }

  return *option;
}

std::atomic_bool going{true};
void check_input() {
  register_keys();

  while (going) {
    auto option = parse_input();
    if (option == Option::Quit) {
      going = false;
    }
  }
}

size_t largest_match(const mi::PossibleMatch &possible_match) {
  if (possible_match.matches.empty()) return 0;

  return std::max_element(possible_match.matches.begin(),
                          possible_match.matches.end(),
                          [](const mi::Match &lhs, const mi::Match &rhs) {
                            return lhs.indices.size() < rhs.indices.size();
                          })
      ->indices.size();
}

static bool match_sort_algorithm(const mi::PossibleMatch &lhs,
                                 const mi::PossibleMatch &rhs) {
  size_t lhs_size = largest_match(lhs);
  size_t rhs_size = largest_match(rhs);
  if (lhs_size > rhs_size) {
    return true;
  } else if (lhs_size == rhs_size) {
    if (lhs_size == 3) {
      if (lhs.matches.size() > rhs.matches.size()) {
        return true;
      } else if (lhs.matches.size() == rhs.matches.size()) {
        return lhs.pos.row > rhs.pos.row;
      } else {
        return false;
      }
    } else {
      return lhs.pos.row < rhs.pos.row;
    }
  } else {
    return false;
  }
}

size_t separated_match(const std::vector<mi::PossibleMatch> &matches,
                       size_t min_size) {
  auto it = std::find_if(matches.begin(), matches.end(),
                         [min_size](const mi::PossibleMatch &match) {
                           return largest_match(match) < min_size;
                         });

  return std::distance(matches.begin(), it);
}

static mi::Board update_board(const mi::PossibleMatch &match,
                              const mi::GridLayout &grid, mi::Board board) {
  mi::swap_gems(match.move_dir, grid, match.pos, board);

  auto matches = mi::find_matches(board, grid);

  auto move_dir = match.move_dir;
  auto pos = match.pos;
  while (!matches.empty()) {
    remove_matches(board, grid, matches, move_dir, pos,
                   mi::RemoveType::Upgrade);
    move_dir = mi::MoveDir::None;

    auto new_matches = find_matches(board, grid);
    while (!new_matches.empty()) {
      remove_matches(board, grid, new_matches, move_dir, pos,
                     mi::RemoveType::Upgrade);
      new_matches = find_matches(board, grid);
    }

    drop_gems(board, grid);

    matches = find_matches(board, grid);
  }

  return board;
}

mi::PossibleMatch calculate_best_match(
    const std::vector<mi::PossibleMatch> &matches, const mi::Board &board,
    const mi::GridLayout &grid) {
  // Look into the future to see which current match is the best option

  size_t end = separated_match(matches, 5);
  if (end == 0) {
    end = separated_match(matches, 4);
    if (end == 0) {
      end = matches.size();
    }
  }

  if (end == 1) {
    return matches.front();
  }

  std::vector<mi::PossibleMatch> best_matches(end);
  for (size_t i = 0; i < end; i++) {
    auto updated_board = update_board(matches[i], grid, board);
    auto next_matches =
        mi::find_possible_matches(updated_board, grid, mi::RemoveType::Upgrade);
    if (next_matches.empty()) continue;

    std::partial_sort(next_matches.begin(), next_matches.begin() + 1,
                      next_matches.end(), match_sort_algorithm);
    best_matches[i] = next_matches.front();
  }

  std::vector<mi::PossibleMatch> best_match(1);
  std::partial_sort_copy(best_matches.begin(), best_matches.end(),
                         best_match.begin(), best_match.end(),
                         match_sort_algorithm);

  if (largest_match(best_match.front()) == 3) {
    return matches.front();
  }

  auto it =
      std::find(best_matches.begin(), best_matches.end(), best_match.front());

  const size_t index = std::distance(best_matches.begin(), it);
  return matches[index];
}

void print_match(const mi::PossibleMatch &possible_match) {
  fmt::print(
      "Location r {} c {} dir {:<5} Largest match: {} Number of matches: "
      "{}\n",
      possible_match.pos.row, possible_match.pos.col,
      (possible_match.move_dir == mi::MoveDir::Down ? "down" : "right"),
      largest_match(possible_match), possible_match.matches.size());
}

static bool are_equal(const cv::Mat &lhs, const cv::Mat &rhs) {
  if (lhs.rows != rhs.rows || lhs.cols != rhs.cols) return false;

  cv::Mat difference = lhs - rhs;
  size_t size = difference.rows * difference.cols;
  cv::Vec3b zero{0, 0, 0};
  for (size_t i = 0; i < size; i++) {
    if (difference.at<cv::Vec3b>(i) != zero) return false;
  }

  return true;
}

static size_t number_of_empty_spaces(const mi::Board &board) {
  return std::count_if(board.begin(), board.end(), [](const mi::Piece &piece) {
    return piece.type == mi::Piece::Type::Empty;
  });
}

static void reset_mouse(int x, int y) {
  cheat::move_mouse(x + 45, y + 45);
  cheat::click_mouse();

  std::this_thread::sleep_for(std::chrono::milliseconds{50});

  cheat::move_mouse(0, 0);
}

int main() {
  static constexpr char window_name_k[] = "GemsOfWar";
  static constexpr mi::GridLayout grid{8, 8};

  std::thread key_monitor(check_input);

  auto screen_pos = cheat::screen_position(window_name_k);
  auto board_pos = cheat::board_position();

  cv::Mat prev_window;
  cv::Mat window;
  while (going) {
    size_t count{0};
    do {
      window.copyTo(prev_window);
      auto screenshot = cheat::take_screenshot();
      cv::cvtColor(screenshot(screen_pos), window, cv::COLOR_BGRA2BGR);
      if (prev_window.empty()) window.copyTo(prev_window);

      count++;
      if (count > 20) {
        fmt::print("Resetting mouse\n");
        reset_mouse(screen_pos.x + board_pos.x, screen_pos.y + board_pos.y);
        count = 0;
      }
    } while (!are_equal(prev_window(board_pos), window(board_pos)) && going);

    if (!going) break;

    auto board = cheat::read_board(window);
    size_t empty_spaces = number_of_empty_spaces(board);

    if (empty_spaces > 5) {
      fmt::print("Game finished, clicking\n");

      cheat::move_mouse(screen_pos.x + 920, screen_pos.y + 864);
      cheat::click_mouse();
      continue;
    }

    if (empty_spaces) {
      std::time_t t = std::time(nullptr);
      auto file_name =
          fmt::format("screenshot_{:%d_%H_%M_%S}.png", fmt::localtime(t));
      cv::imwrite(file_name, window);
    }

    auto matches =
        mi::find_possible_matches(board, grid, mi::RemoveType::Upgrade);

    std::optional<mi::PossibleMatch> best_match;
    if (!matches.empty()) {
      std::sort(matches.begin(), matches.end(), match_sort_algorithm);
      best_match = calculate_best_match(matches, board, grid);
      fmt::print("Best match:\n");
      print_match(*best_match);
    }

    if (best_match) {
      cheat::move_piece(best_match->move_dir, best_match->pos, screen_pos);
      best_match.reset();
    }
  }

  key_monitor.join();
}