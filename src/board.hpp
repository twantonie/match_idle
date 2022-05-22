#ifndef MATCH_IDLE_BOARD_HPP
#define MATCH_IDLE_BOARD_HPP

#include <vector>

#include "piece.hpp"

namespace match_idle {

struct GridLayout {
  uint32_t rows = 8;
  uint32_t cols = 8;
};

enum class MoveDir { None, Up, Down, Left, Right };

struct Point {
  uint32_t row;
  uint32_t col;

  bool operator==(const Point &other) const {
    return row == other.row && col == other.col;
  }
};

bool is_move_direction_valid(MoveDir move_dir, const GridLayout &grid,
                             Point pos);

struct Match {
  Match() = default;
  Match(std::vector<size_t> _indices, Piece::Type _type = Piece::Type::Empty)
      : indices(_indices), type(_type) {}

  std::vector<size_t> indices;
  Piece::Type type{Piece::Type::Empty};
  Piece::Special special{Piece::Special::None};
};

std::vector<Match> find_matches(const std::vector<Piece> &board,
                                const GridLayout &grid);

enum class RemoveType { None, Special, Upgrade };

void remove_matches(std::vector<Piece> &board, const GridLayout &grid,
                    const std::vector<Match> &matches,
                    MoveDir move_dir = MoveDir::None, Point pos = {},
                    RemoveType remove_type = RemoveType::None);

void drop_gems(std::vector<Piece> &board, const GridLayout &grid);

bool swap_gems(MoveDir move_dir, const GridLayout &grid, Point pos,
               std::vector<Piece> &board);

struct PossibleMatch {
  Point pos;
  MoveDir move_dir;
  std::vector<Match> matches;
};

std::vector<PossibleMatch> find_possible_matches(
    const std::vector<Piece> &board, const GridLayout &grid,
    RemoveType remove_type);

}  // namespace match_idle

#endif