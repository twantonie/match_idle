#ifndef MATCH_IDLE_MATCH_WINDOW_HPP
#define MATCH_IDLE_MATCH_WINDOW_HPP

#include <centurion.hpp>
#include <random>

#include "board.hpp"

namespace match_idle {

class MatchArea {
 public:
  MatchArea() = default;
  MatchArea(cen::irect area, std::mt19937 gen);

  void handle_events(cen::event &event);
  void render(cen::renderer &renderer) const;

  cen::irect area() const { return _area; }
  void area(cen::irect area) { _update_area(area); }

  bool game_over() const { return _possible_matches.empty(); }

 private:
  void _update_area(cen::irect area);

  cen::irect _area;
  std::mt19937 _gen;
  std::vector<Piece> _board;
  std::vector<PossibleMatch> _possible_matches;

  cen::ipoint _selected_gem{-1, -1};
  cen::ipoint _mouse_pressed_loc{0, 0};
  MoveDir _move_dir{MoveDir::None};

  static constexpr GridLayout grid{8, 8};
};

}  // namespace match_idle

#endif