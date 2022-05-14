#include "board.hpp"

#include <algorithm>

namespace match_idle {

// Calculates matches and returns them as a vector of matches, and each match is
// a number of indices

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

std::vector<Match> find_matches(const std::vector<Piece> &board,
                                const GridLayout &grid) {
  std::vector<Match> matches;

  for (size_t i = 0; i < board.size(); i++) {
    const auto &gem = board[i];
    if (gem.type == Piece::Type::Empty) continue;

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

static void drop_gems(std::vector<Piece> &board, const GridLayout &grid) {
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

void remove_matches(std::vector<Piece> &board, const GridLayout &grid,
                    const std::vector<Match> &matches) {
  for (auto &match : matches) {
    for (auto indice : match.indices) {
      board[indice].type = Piece::Type::Empty;
    }

    // TODO: Apply special of gems if any

    // TODO: Need to insert special gems, closest to where the match was made.
    // But in some instances you don't want this to happen, for instance at
    // startup
  }

  drop_gems(board, grid);
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

void swap_gems(MoveDir move_dir, const GridLayout &grid, Point pos,
               std::vector<Piece> &board) {
  uint32_t index = pos.col + pos.row * grid.cols;
  uint32_t other_index{};

  if (move_dir == MoveDir::Right || move_dir == MoveDir::Left) {
    other_index = index + (move_dir == MoveDir::Right ? 1 : -1);
  } else if (move_dir == MoveDir::Down) {
    other_index = index + grid.cols;
  } else {
    other_index = index - grid.cols;
  }

  std::swap(board[index], board[other_index]);
}

PossibleMatch find_possible_match(std::vector<Piece> &board,
                                  const GridLayout &grid, Point pos,
                                  MoveDir move_dir) {
  PossibleMatch possible_match;
  possible_match.pos = pos;
  possible_match.move_dir = move_dir;

  if (!is_move_direction_valid(move_dir, grid, pos)) return possible_match;

  swap_gems(move_dir, grid, pos, board);

  std::vector<Match> matches = find_matches(board, grid);
  if (matches.empty()) {
    // Move gems back into normal position
    swap_gems(move_dir, grid, pos, board);
  }

  while (!matches.empty()) {
    remove_matches(board, grid, matches);

    possible_match.matches.insert(possible_match.matches.begin(),
                                  matches.begin(), matches.end());

    matches = find_matches(board, grid);
  }

  return possible_match;
}

std::vector<PossibleMatch> find_possible_matches(
    const std::vector<Piece> &board, const GridLayout &grid) {
  std::vector<PossibleMatch> possible_matches;

  std::vector<Piece> board_copy = board;

  const auto add_match = [&](PossibleMatch match) {
    if (!match.matches.empty()) {
      possible_matches.push_back(match);

      // Reset board when we found a match because we'll have messed with it
      board_copy = board;
    }
  };

  for (uint32_t r = 0; r < grid.rows; r++) {
    for (uint32_t c = 0; c < grid.cols; c++) {
      add_match(
          find_possible_match(board_copy, grid, Point{r, c}, MoveDir::Right));
      add_match(
          find_possible_match(board_copy, grid, Point{r, c}, MoveDir::Down));
    }
  }

  return possible_matches;
}

}  // namespace match_idle