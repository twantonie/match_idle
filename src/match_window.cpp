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

// Calculates matches and returns them as a vector of matches, and each match is
// a number of indices

static Match calculate_match(std::vector<size_t> &indices) {
  Match match;

  if (indices.size() == 6) {
    // TODO: Super special
    match.special = Gem::Special::HyperCube;
  } else if (indices.size() == 5) {
    match.special = Gem::Special::HyperCube;
  } else if (indices.size() == 4) {
    match.special = Gem::Special::Explosive;
  }

  match.indices = std::move(indices);

  return match;
}

std::vector<Match> find_matches(const std::vector<Gem> &board,
                                const GridLayout &grid) {
  std::vector<Match> matches;

  for (size_t i = 0; i < board.size(); i++) {
    const auto &gem = board[i];

    std::vector<size_t> horizontal_match{i};
    const size_t x_max = grid.cols * ((i + grid.cols) / grid.cols);
    for (size_t x = i + 1; x < x_max; x++) {
      if (board[x].type != gem.type) break;

      horizontal_match.push_back(x);
    }

    std::vector<size_t> vertical_match{i};
    for (size_t y = i + grid.cols; y < grid.cols * grid.rows; y += grid.cols) {
      if (board[y].type != gem.type) break;

      vertical_match.push_back(y);
    }

    Match match;
    if (horizontal_match.size() > 2) {
      match = calculate_match(horizontal_match);
    } else if (vertical_match.size() > 2) {
      match = calculate_match(vertical_match);
    }

    if (!match.indices.empty()) {
      matches.push_back(std::move(match));
    }
  }

  // TODO: Filter matches

  return matches;
}

MatchArea::MatchArea(cen::irect area, std::mt19937 gen) : _gen(gen) {
  _board = fill_board(_gen, grid.rows * grid.cols);

  _update_area(area);
}

void MatchArea::handle_events(cen::event &event) {
  if (event.is<cen::mouse_button_event>()) {
    const auto &ev = event.get<cen::mouse_button_event>();

    // Check if event is within our area
    if (ev.x() < _area.x() || ev.x() >= _area.x() + _area.width() ||
        ev.y() < _area.y() || ev.y() >= _area.y() + _area.height()) {
      return;
    }

    // Check which rectangle we clicked on
    size_t col_index = (ev.x() - _area.x()) * grid.cols / _area.width();
    size_t row_index = (ev.y() - _area.y()) * grid.rows / _area.height();

    _selected_index = col_index + row_index * grid.cols;

    if (ev.released()) {
      const auto x_diff = ev.x() - _mouse_pressed_loc.x();
      const auto y_diff = ev.y() - _mouse_pressed_loc.y();
      constexpr auto min_delta = 15;

      if (std::sqrt(x_diff * x_diff + y_diff * y_diff) > min_delta) {
        if (std::abs(x_diff) > std::abs(y_diff)) {
          _move_dir = (x_diff > 0 ? MoveDir::Right : MoveDir::Left);
        } else {
          _move_dir = (y_diff > 0 ? MoveDir::Down : MoveDir::Up);
        }
      }

      // XXX: Update board if move direction is valid for pressed gem
    } else if (ev.pressed()) {
      _mouse_pressed_loc = cen::ipoint(ev.x(), ev.y());
      _move_dir = MoveDir::None;
    }
  }
}

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

static void draw_line_in_move_dir(cen::renderer &renderer, MoveDir dir,
                                  const cen::ipoint &point) {
  constexpr auto length = 10;

  if (dir == MoveDir::None) return;

  cen::ipoint to = point;
  switch (dir) {
    case MoveDir::Left:
      to.set_x(to.x() - length);
      break;
    case MoveDir::Right:
      to.set_x(to.x() + length);
      break;
    case MoveDir::Up:
      to.set_y(to.y() - length);
      break;
    case MoveDir::Down:
      to.set_y(to.y() + length);
      break;
  }

  renderer.draw_line(point, to);
}

void MatchArea::render(cen::renderer &renderer) const {
  auto matches = find_matches(_board, grid);

  renderer.set_color(cen::colors::black);

  // Draw vertical lines
  for (size_t i = 0; i < grid.rows + 1; i++) {
    int y = _area.y() + _area.height() / grid.rows * i;
    renderer.draw_line(cen::ipoint{_area.x(), y},
                       cen::ipoint{_area.x() + _area.width(), y});
  }

  // Draw horizontal lines
  for (size_t i = 0; i < grid.cols + 1; i++) {
    int x = _area.x() + _area.width() / grid.cols * i;
    renderer.draw_line(cen::ipoint{x, _area.y()},
                       cen::ipoint{x, _area.y() + _area.height()});
  }

  // Draw selected index
  if (_selected_index != -1) {
    int c = _selected_index % grid.rows;
    int r = _selected_index / grid.cols;

    int x = _area.x() + _area.width() / grid.cols * c;
    int y = _area.y() + _area.height() / grid.rows * r;

    const int width = _area.width() / grid.cols;
    const int height = _area.height() / grid.rows;

    renderer.set_color(cen::colors::black);
    renderer.fill_rect(cen::irect(x, y, width, height));
  }

  // Draw gems
  const int width = _area.width() / grid.cols - 8;
  const int height = _area.height() / grid.rows - 8;
  for (size_t r = 0; r < grid.rows; r++) {
    for (size_t c = 0; c < grid.cols; c++) {
      int x = _area.x() + _area.width() / grid.cols * c + 4;
      int y = _area.y() + _area.height() / grid.rows * r + 4;

      set_color(_board[r * grid.cols + c].type, renderer);
      renderer.fill_rect(cen::irect(x, y, width, height));

      renderer.set_color(cen::colors::black);
      renderer.draw_rect(cen::irect(x, y, width, height));
    }
  }

  // Draw circle around mouse location
  renderer.set_color(cen::colors::red);
  renderer.draw_circle(_mouse_pressed_loc, 10.f);

  draw_line_in_move_dir(renderer, _move_dir, _mouse_pressed_loc);
}

void MatchArea::_update_area(cen::irect area) {
  _area = area;

  // TODO: Check that width and height can be equally divided by grid
  // width/height
}

}  // namespace match_idle