#include "board.hpp"

#include <fmt/core.h>

#include <algorithm>
#include <numeric>

namespace match_idle {

// Calculates matches and returns them as a vector of matches, and each match is
// a number of indices

void print_board(const Board &board, const GridLayout &grid) {
  fmt::print("Board board{{\n");
  for (size_t r = 0; r < grid.rows; r++) {
    fmt::print("  ");
    for (size_t c = 0; c < grid.cols; c++) {
      size_t index = r * grid.cols + c;
      fmt::print("{{Type::{}}}, ", to_string(board[index].type));
    }
    fmt::print("\n");
  }
  fmt::print("}};\n");
}

static void calculate_match(std::vector<size_t> &indices,
                            std::vector<Match> &matches, Piece::Type type) {
  if (indices.size() < 3) return;

  matches.push_back({});
  Match &match = matches.back();
  match.type = type;

  if (indices.size() == 6) {
    // TODO: Super special
    match.special = Piece::Special::HyperCube;
  } else if (indices.size() == 5) {
    match.special = Piece::Special::HyperCube;
  } else if (indices.size() == 4) {
    match.special = Piece::Special::Explosive;
  }

  match.indices = std::move(indices);
}

bool horizontal(const Match &match) {
  return match.indices[0] + 1 == match.indices[1];
}

static void erase_duplicate_indices(std::vector<size_t> &indices) {
  std::sort(indices.begin(), indices.end());
  auto it = std::unique(indices.begin(), indices.end());
  indices.erase(it, indices.end());
}

void filter_matches(std::vector<Match> &matches) {
  for (auto it = matches.begin(); it != matches.end(); it++) {
    auto start = it + 1;

    while (start != matches.end()) {
      auto res = std::find_if(start, matches.end(), [&it](const Match &match) {
        for (auto i : it->indices) {
          auto i_res = std::find(match.indices.begin(), match.indices.end(), i);

          if (i_res != match.indices.end()) return true;
        }

        return false;
      });

      if (res == matches.end()) break;

      // XXX: We need to take into account more special cases
      if (horizontal(*it) != horizontal(*res)) {
        it->special = Piece::Special::Lightning;
      }

      // Add indices of matching matches together
      it->indices.insert(it->indices.end(), res->indices.begin(),
                         res->indices.end());

      start = matches.erase(res);
    }
  }

  for (auto &match : matches) {
    erase_duplicate_indices(match.indices);
  }
}

std::vector<Match> find_matches(const Board &board, const GridLayout &grid) {
  std::vector<Match> matches;

  for (size_t i = 0; i < board.size(); i++) {
    const auto &gem = board[i];
    if (!gem.is_matchable()) continue;

    std::vector<size_t> horizontal_match{i};
    const size_t x_max = grid.cols * ((i + grid.cols) / grid.cols);
    for (size_t x = i + 1; x < x_max; x++) {
      if (board[x].type != gem.type) break;

      horizontal_match.push_back(x);
    }

    calculate_match(horizontal_match, matches, gem.type);

    std::vector<size_t> vertical_match{i};
    for (size_t y = i + grid.cols; y < grid.cols * grid.rows; y += grid.cols) {
      if (board[y].type != gem.type) break;

      vertical_match.push_back(y);
    }

    calculate_match(vertical_match, matches, gem.type);
  }

  filter_matches(matches);

  return matches;
}

void drop_gems(Board &board, const GridLayout &grid) {
  // Check column by column, start from bottom
  for (size_t c = 0; c < grid.cols; c++) {
    int empty_row = -1;
    for (int r = grid.rows - 1; r > -1; r--) {
      auto &gem = board[r * grid.cols + c];

      if (gem.type == Piece::Type::Empty) {
        if (empty_row == -1) empty_row = r;
        continue;
      }

      if (empty_row != -1) {
        std::swap(board[empty_row * grid.cols + c], gem);
        empty_row--;
      }
    }
  }
}

static size_t calculate_index(Point pos, const GridLayout &grid) {
  return pos.col + pos.row * grid.cols;
}

static size_t calculate_other_index(size_t index, MoveDir move_dir,
                                    const GridLayout &grid) {
  if (move_dir == MoveDir::Right || move_dir == MoveDir::Left) {
    return index + (move_dir == MoveDir::Right ? 1 : -1);
  } else if (move_dir == MoveDir::Down) {
    return index + grid.cols;
  } else {
    return index - grid.cols;
  }
}

static size_t get_remove_type_location(const GridLayout &grid, MoveDir move_dir,
                                       Point pos, const Match &match) {
  const auto indices = match.indices;

  if (move_dir != MoveDir::None) {
    size_t index = calculate_index(pos, grid);
    auto it = std::find(indices.begin(), indices.end(), index);
    if (it != indices.end()) {
      return index;
    }

    size_t other_index = calculate_other_index(index, move_dir, grid);
    it = std::find(indices.begin(), indices.end(), other_index);
    if (it != indices.end()) {
      return other_index;
    }

    // Something weird is going on if a match is found that doesn't relate to
    // the move. Fall through just to find an index
  }

  // If there is no direction the new piece spawns in the middle. For 4 in a
  // row vertically it is the third piece that is converted, horizontally this
  // is the case too. With third piece I mean third starting from the left or
  // top respectively. With a cross match (five 3 both ways) that fell
  // together the match is top right |- for such a shape. Same for T shape. It
  // seems to be the bottom right for more intricate matches

  // XXX: This is not gonna do the trick for more complicated shapes, but
  // should work for simple horizontal and vertical matches
  size_t total = std::accumulate(indices.begin(), indices.end(), 0);
  size_t avg = std::ceil(1.0 * total / indices.size());
  return *std::find_if(indices.begin(), indices.end(),
                       [avg](size_t index) { return index >= avg; });
}

static Piece::Type upgrade_piece_type(Piece::Type type) {
  uint32_t raw_type = static_cast<uint32_t>(type);
  raw_type++;
  return static_cast<Piece::Type>(raw_type);
}

static Piece::Special match_size_to_special(size_t nr_indices) {
  switch (nr_indices) {
    case 4:
      return Piece::Special::Explosive;
    case 5:
      return Piece::Special::Lightning;
    case 6:
      return Piece::Special::HyperCube;
    default:
      return Piece::Special::None;
  }
}

void remove_matches(Board &board, const GridLayout &grid,
                    const std::vector<Match> &matches, MoveDir move_dir,
                    Point pos, RemoveType remove_type) {
  for (auto &match : matches) {
    // TODO: Apply special of gems if any

    for (auto indice : match.indices) {
      board[indice].type = Piece::Type::Empty;
    }

    if (remove_type == RemoveType::Upgrade) {
      size_t upgrade_index =
          get_remove_type_location(grid, move_dir, pos, match);
      board[upgrade_index].type = upgrade_piece_type(match.type);
    } else if (remove_type == RemoveType::Special && match.indices.size() > 3) {
      size_t upgrade_index =
          get_remove_type_location(grid, move_dir, pos, match);
      board[upgrade_index].type = upgrade_piece_type(match.type);
      board[upgrade_index].special =
          match_size_to_special(match.indices.size());
    }
  }
}

bool is_move_direction_valid(MoveDir move_dir, const GridLayout &grid,
                             Point pos) {
  if (move_dir == MoveDir::None) return false;
  if (move_dir == MoveDir::Right && pos.col == grid.cols - 1) return false;
  if (move_dir == MoveDir::Left && pos.col == 0) return false;
  if (move_dir == MoveDir::Down && pos.row == grid.rows - 1) return false;
  if (move_dir == MoveDir::Up && pos.col == 0) return false;

  return true;
}

bool swap_gems(MoveDir move_dir, const GridLayout &grid, Point pos,
               Board &board) {
  const uint32_t index = calculate_index(pos, grid);
  const uint32_t other_index = calculate_other_index(index, move_dir, grid);

  if (board[index].is_movable() && board[other_index].is_movable()) {
    std::swap(board[index], board[other_index]);
    return true;
  } else {
    return false;
  }
}

static PossibleMatch find_possible_match(Board &board, const GridLayout &grid,
                                         Point pos, MoveDir move_dir,
                                         RemoveType remove_type) {
  PossibleMatch possible_match;
  possible_match.pos = pos;
  possible_match.move_dir = move_dir;

  if (!is_move_direction_valid(move_dir, grid, pos)) return possible_match;

  if (!swap_gems(move_dir, grid, pos, board)) return possible_match;

  std::vector<Match> matches = find_matches(board, grid);
  if (matches.empty()) {
    // Move gems back into normal position
    swap_gems(move_dir, grid, pos, board);
  }

  while (!matches.empty()) {
    remove_matches(board, grid, matches, move_dir, pos, remove_type);
    move_dir = MoveDir::None;

    if (remove_type != RemoveType::None) {
      auto new_matches = find_matches(board, grid);
      while (!new_matches.empty()) {
        possible_match.matches.insert(possible_match.matches.begin(),
                                      new_matches.begin(), new_matches.end());
        remove_matches(board, grid, new_matches, move_dir, pos, remove_type);
        new_matches = find_matches(board, grid);
      }
    }

    drop_gems(board, grid);

    possible_match.matches.insert(possible_match.matches.begin(),
                                  matches.begin(), matches.end());

    matches = find_matches(board, grid);
  }

  return possible_match;
}

std::vector<PossibleMatch> find_possible_matches(const Board &board,
                                                 const GridLayout &grid,
                                                 RemoveType remove_type) {
  std::vector<PossibleMatch> possible_matches;

  Board board_copy = board;

  const auto add_match = [&](PossibleMatch match) {
    if (!match.matches.empty()) {
      possible_matches.push_back(match);

      // Reset board when we found a match because we'll have messed with it
      board_copy = board;
    }
  };

  for (uint32_t r = 0; r < grid.rows; r++) {
    for (uint32_t c = 0; c < grid.cols; c++) {
      add_match(find_possible_match(board_copy, grid, Point{r, c},
                                    MoveDir::Right, remove_type));
      add_match(find_possible_match(board_copy, grid, Point{r, c},
                                    MoveDir::Down, remove_type));
    }
  }

  return possible_matches;
}

}  // namespace match_idle