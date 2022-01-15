#include "match_window.hpp"

namespace match_idle {

void MatchArea::handle_events(cen::event &) {}

void MatchArea::render(cen::renderer &renderer) const {
  renderer.set_color(cen::colors::black);

  // Draw vertical lines
  for (size_t i = 0; i < grid_cols + 1; i++) {
    int y = _area.y() + _area.height() / grid_cols * i;
    renderer.draw_line(cen::ipoint{_area.x(), y},
                       cen::ipoint{_area.x() + _area.width(), y});
  }

  // Draw horizontal lines
  for (size_t i = 0; i < grid_rows + 1; i++) {
    int x = _area.x() + _area.width() / grid_rows * i;
    renderer.draw_line(cen::ipoint{x, _area.y()},
                       cen::ipoint{x, _area.y() + _area.height()});
  }
}

void MatchArea::_update_area(cen::irect area) {
  _area = area;

  // TODO: Check that width and height can be equally divided by grid
  // width/height
}

}  // namespace match_idle