#include "match_window.hpp"

namespace match_idle {

static std::vector<Gem> fill_board(std::mt19937 &gen, size_t nr_values) {
  std::uniform_int_distribution<uint32_t> dist(0, 6);

  std::vector<Gem> board;
  board.resize(nr_values);

  for (size_t i = 0; i < nr_values; i++) {
    board[i] = Gem(static_cast<Gem::Type>(dist(gen)));
  }

  return board;
}

MatchArea::MatchArea(cen::irect area, std::mt19937 gen) : _gen(gen) {
  _board = fill_board(_gen, grid_rows * grid_cols);
  _update_area(area);
}

void MatchArea::handle_events(cen::event &) {}

static void set_color(Gem::Type type, cen::renderer &renderer) {
  switch (type) {
    case Gem::Type::Red:
      renderer.set_color(cen::colors::red);
      break;
    case Gem::Type::Blue:
      renderer.set_color(cen::colors::blue);
      break;
    case Gem::Type::Orange:
      renderer.set_color(cen::colors::orange);
      break;
    case Gem::Type::Purple:
      renderer.set_color(cen::colors::purple);
      break;
    case Gem::Type::Green:
      renderer.set_color(cen::colors::green);
      break;
    case Gem::Type::Yellow:
      renderer.set_color(cen::colors::yellow);
      break;
    case Gem::Type::White:
      renderer.set_color(cen::colors::white);
      break;
  }
}

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

  // Draw gems
  const int width = _area.width() / grid_rows - 8;
  const int height = _area.height() / grid_cols - 8;
  for (size_t r = 0; r < grid_rows; r++) {
    for (size_t c = 0; c < grid_cols; c++) {
      int x = _area.x() + _area.width() / grid_rows * r + 4;
      int y = _area.y() + _area.height() / grid_cols * c + 4;

      set_color(_board[r * grid_cols + c].type, renderer);
      renderer.fill_rect(cen::irect(x, y, width, height));

      renderer.set_color(cen::colors::black);
      renderer.draw_rect(cen::irect(x, y, width, height));
    }
  }
}

void MatchArea::_update_area(cen::irect area) {
  _area = area;

  // TODO: Check that width and height can be equally divided by grid
  // width/height
}

}  // namespace match_idle