#include "match_window.hpp"

namespace match_idle {

std::uniform_int_distribution<uint32_t> Piece::dist{0, 6};

static std::vector<Piece> fill_board(std::mt19937 &gen, size_t nr_values) {
  std::vector<Piece> board;
  board.resize(nr_values);

  for (size_t i = 0; i < nr_values; i++) {
    board[i] = Piece(gen);
  }

  return board;
}

static void fill_empty_gems(std::vector<Piece> &board, std::mt19937 &gen) {
  for (auto &gem : board) {
    if (gem.type != Piece::Type::Empty) continue;

    gem = Piece(gen);
  }
}

static void update_board(GridLayout const &grid, std::vector<Piece> &board,
                         std::mt19937 &gen) {
  std::vector<Match> matches;

  do {
    matches = find_matches(board, grid);
    remove_matches(board, grid, matches);
    fill_empty_gems(board, gen);
  } while (!matches.empty());
}

MatchArea::MatchArea(cen::irect area, std::mt19937 gen) : _gen(gen) {
  _board = fill_board(_gen, grid.rows * grid.cols);

  update_board(grid, _board, _gen);
  _possible_matches = find_possible_matches(_board, grid);
  _update_area(area);
}

void MatchArea::handle_events(cen::event &event) {
  if (event.is<cen::mouse_button_event>()) {
    const auto &ev = event.get<cen::mouse_button_event>();

    if (ev.released() && _selected_gem.x() != -1) {
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

      auto point = Point{static_cast<uint32_t>(_selected_gem.y()),
                         static_cast<uint32_t>(_selected_gem.x())};

      if (is_move_direction_valid(_move_dir, grid, point)) {
        auto board_copy = _board;
        swap_gems(_move_dir, grid, point, board_copy);

        auto matches = find_matches(board_copy, grid);

        if (!matches.empty()) {
          _board = board_copy;
          update_board(grid, _board, _gen);
          _possible_matches = find_possible_matches(_board, grid);
        }
      }

      _selected_gem = cen::ipoint{-1, -1};

    } else if (ev.pressed()) {
      // Check if event is within our area
      if (ev.x() < _area.x() || ev.x() >= _area.x() + _area.width() ||
          ev.y() < _area.y() || ev.y() >= _area.y() + _area.height()) {
        return;
      }

      // Check which rectangle we clicked on
      int col_index = (ev.x() - _area.x()) * grid.cols / _area.width();
      int row_index = (ev.y() - _area.y()) * grid.rows / _area.height();

      _selected_gem = cen::ipoint{col_index, row_index};

      _mouse_pressed_loc = cen::ipoint(ev.x(), ev.y());
      _move_dir = MoveDir::None;
    }
  }
}

static void set_color(Piece::Type type, cen::renderer &renderer) {
  switch (type) {
    case Piece::Type::Red:
      renderer.set_color(cen::colors::red);
      break;
    case Piece::Type::Blue:
      renderer.set_color(cen::colors::blue);
      break;
    case Piece::Type::Orange:
      renderer.set_color(cen::colors::orange);
      break;
    case Piece::Type::Purple:
      renderer.set_color(cen::colors::purple);
      break;
    case Piece::Type::Green:
      renderer.set_color(cen::colors::green);
      break;
    case Piece::Type::Yellow:
      renderer.set_color(cen::colors::yellow);
      break;
    case Piece::Type::White:
      renderer.set_color(cen::colors::white);
      break;
    case Piece::Type::Empty:
      break;
  }
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
  if (_selected_gem.x() != -1) {
    int x = _area.x() + _area.width() / grid.cols * _selected_gem.x();
    int y = _area.y() + _area.height() / grid.rows * _selected_gem.y();

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
}

void MatchArea::_update_area(cen::irect area) {
  _area = area;

  // TODO: Check that width and height can be equally divided by grid
  // width/height
}

}  // namespace match_idle