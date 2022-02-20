#include "match_window.hpp"

namespace match_idle {

const std::uniform_int_distribution<uint32_t> Gem::dist{0, 6};

static std::vector<Gem> fill_board(std::mt19937 &gen, size_t nr_values) {
  std::vector<Gem> board;
  board.resize(nr_values);

  for (size_t i = 0; i < nr_values; i++) {
    board[i] = Gem(gen);
  }

  return board;
}

// Calculates matches and returns them as a vector of matches, and each match is
// a number of indices

static void calculate_match(std::vector<size_t> &indices,
                            std::vector<Match> &matches, Gem::Type type) {
  if (indices.size() < 3) return;

  matches.push_back({});
  Match &match = matches.back();
  match.type = type;

  if (indices.size() == 6) {
    // TODO: Super special
    match.special = Gem::Special::HyperCube;
  } else if (indices.size() == 5) {
    match.special = Gem::Special::HyperCube;
  } else if (indices.size() == 4) {
    match.special = Gem::Special::Explosive;
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
        it->special = Gem::Special::Lightning;
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

std::vector<Match> find_matches(const std::vector<Gem> &board,
                                const GridLayout &grid) {
  std::vector<Match> matches;

  for (size_t i = 0; i < board.size(); i++) {
    const auto &gem = board[i];
    if (gem.type == Gem::Type::Empty) continue;

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

static void drop_gems(std::vector<Gem> &board, const GridLayout &grid) {
  // Check column by column, start from bottom
  for (size_t c = 0; c < grid.cols; c++) {
    int empty_row = -1;
    for (int r = grid.rows - 1; r > -1; r--) {
      auto &gem = board[r * grid.cols + c];

      if (gem.type == Gem::Type::Empty) {
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

void remove_matches(std::vector<Gem> &board, const GridLayout &grid,
                    const std::vector<Match> &matches) {
  for (auto &match : matches) {
    for (auto indice : match.indices) {
      board[indice].type = Gem::Type::Empty;
    }

    // TODO: Apply special of gems if any

    // TODO: Need to insert special gems, closest to where the match was made.
    // But in some instances you don't want this to happen, for instance at
    // startup
  }

  drop_gems(board, grid);
}

static bool is_move_direction_valid(MoveDir move_dir, const GridLayout &grid,
                                    cen::ipoint pos) {
  if (move_dir == MoveDir::None) return false;
  if (move_dir == MoveDir::Right && pos.x() == grid.cols - 1) return false;
  if (move_dir == MoveDir::Left && pos.x() == 0) return false;
  if (move_dir == MoveDir::Down && pos.y() == grid.rows - 1) return false;
  if (move_dir == MoveDir::Up && pos.y() == 0) return false;

  return true;
}

static void swap_gems(MoveDir move_dir, const GridLayout &grid, cen::ipoint pos,
                      std::vector<Gem> &board) {
  uint32_t index = pos.x() + pos.y() * grid.cols;
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

PossibleMatch find_possible_match(std::vector<Gem> &board,
                                  const GridLayout &grid, cen::ipoint pos,
                                  MoveDir move_dir) {
  PossibleMatch possible_match{pos, move_dir};

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

std::vector<PossibleMatch> find_possible_matches(const std::vector<Gem> &board,
                                                 const GridLayout &grid) {
  std::vector<PossibleMatch> possible_matches;

  std::vector<Gem> board_copy = board;

  const auto add_match = [&](PossibleMatch match) {
    if (!match.matches.empty()) {
      possible_matches.push_back(match);

      // Reset board when we found a match because we'll have messed with it
      board_copy = board;
    }
  };

  for (int r = 0; r < static_cast<int>(grid.rows); r++) {
    for (int c = 0; c < static_cast<int>(grid.cols); c++) {
      add_match(find_possible_match(board_copy, grid, cen::ipoint{c, r},
                                    MoveDir::Right));
      add_match(find_possible_match(board_copy, grid, cen::ipoint{c, r},
                                    MoveDir::Down));
    }
  }

  return possible_matches;
}

static void fill_empty_gems(std::vector<Gem> &board, std::mt19937 &gen) {
  for (auto &gem : board) {
    if (gem.type != Gem::Type::Empty) continue;

    gem = Gem(gen);
  }
}

static void update_board(GridLayout const &grid, std::vector<Gem> &board,
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

      if (is_move_direction_valid(_move_dir, grid, _selected_gem)) {
        auto board_copy = _board;
        swap_gems(_move_dir, grid, _selected_gem, board_copy);

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