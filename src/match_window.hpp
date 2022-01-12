#ifndef _MATCH_IDLE_MATCH_WINDOW_HPP_
#define _MATCH_IDLE_MATCH_WINDOW_HPP_

#include <centurion.hpp>

namespace match_idle {

class MatchArea {
 public:
  MatchArea(cen::irect area) { update_area_(area); }

  void handle_events(cen::event &event);
  void render(cen::renderer &renderer) const;

  cen::irect area() const { return area_; }
  void area(cen::irect area) { update_area_(area); }

 private:
  void update_area_(cen::irect area);

  cen::irect area_;

  static constexpr int grid_rows = 8;
  static constexpr int grid_cols = 8;
};

}  // namespace match_idle

#endif