#ifndef _MATCH_IDLE_MATCH_WINDOW_HPP_
#define _MATCH_IDLE_MATCH_WINDOW_HPP_

#include <centurion.hpp>

namespace match_idle {

class MatchArea {
 public:
  MatchArea(cen::irect area) { _update_area(area); }

  void handle_events(cen::event &event);
  void render(cen::renderer &renderer) const;

  cen::irect area() const { return _area; }
  void area(cen::irect area) { _update_area(area); }

 private:
  void _update_area(cen::irect area);

  cen::irect _area;

  static constexpr int grid_rows = 8;
  static constexpr int grid_cols = 8;
};

}  // namespace match_idle

#endif