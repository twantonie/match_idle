#include "match_window.hpp"

namespace match_idle {

void MatchArea::handle_events(cen::event &) {}

void MatchArea::render(cen::renderer &renderer) const {
  renderer.set_color(cen::colors::black);

  // Draw vertical lines
  for (size_t i = 0; i < grid_cols + 1; i++) {
    int y = area_.y() + area_.height() / grid_cols * i;
    renderer.draw_line(cen::ipoint{area_.x(), y},
                       cen::ipoint{area_.x() + area_.width(), y});
  }

  // Draw horizontal lines
  for (size_t i = 0; i < grid_rows + 1; i++) {
    int x = area_.x() + area_.width() / grid_rows * i;
    renderer.draw_line(cen::ipoint{x, area_.y()},
                       cen::ipoint{x, area_.y() + area_.height()});
  }
}

void MatchArea::update_area_(cen::irect area) {
  area_ = area;

  // TODO: Check that width and height can be equally divided by grid
  // width/height
}

}  // namespace match_idle