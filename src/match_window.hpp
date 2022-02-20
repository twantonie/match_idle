#ifndef _MATCH_IDLE_MATCH_WINDOW_HPP_
#define _MATCH_IDLE_MATCH_WINDOW_HPP_

#include <centurion.hpp>
#include <random>

namespace match_idle {

// How do I implement the board and all the gems on the board??
// We could do something very simple for now, literally just a type specifier
// and store that in a vector. And then we could handle all the other stuff
// separately

// XXX: Might want to give this a different name
struct Gem {
  static const std::uniform_int_distribution<uint32_t> dist;

  enum class Type : uint32_t {
    Red,
    Blue,
    Yellow,
    Orange,
    Purple,
    Green,
    White,
    Empty
  };
  enum class Special : uint32_t { None, Explosive, Lightning, HyperCube };

  Gem() = default;
  Gem(Type type_, Special special_ = Special::None)
      : type(type_), special(special_) {}
  explicit Gem(std::mt19937 &gen) : type(static_cast<Type>(dist(gen))) {}

  bool operator==(const Gem &rhs) const {
    return type == rhs.type && special == rhs.special;
  }

  friend std::ostream &operator<<(std::ostream &stream, const Gem &gem) {
    return stream << static_cast<uint32_t>(gem.type);
  }

  Type type;
  Special special{Special::None};
};

struct GridLayout {
  uint32_t rows = 8;
  uint32_t cols = 8;
};

enum class MoveDir { None, Up, Down, Left, Right };

struct Match {
  std::vector<size_t> indices;
  Gem::Type type;
  Gem::Special special{Gem::Special::None};
};

std::vector<Match> find_matches(const std::vector<Gem> &board,
                                const GridLayout &grid);

void remove_matches(std::vector<Gem> &board, const GridLayout &grid,
                    const std::vector<Match> &matches);

struct PossibleMatch {
  cen::ipoint pos;
  MoveDir move_dir;
  std::vector<Match> matches;
};

std::vector<PossibleMatch> find_possible_matches(const std::vector<Gem> &board,
                                                 const GridLayout &grid);

class MatchArea {
 public:
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
  std::vector<Gem> _board;
  std::vector<PossibleMatch> _possible_matches;

  cen::ipoint _selected_gem{-1, -1};
  cen::ipoint _mouse_pressed_loc{0, 0};
  MoveDir _move_dir{MoveDir::None};

  static constexpr GridLayout grid{8, 8};
};

}  // namespace match_idle

#endif