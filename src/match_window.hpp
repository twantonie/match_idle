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
  enum class Type : uint32_t {
    Red,
    Blue,
    Yellow,
    Orange,
    Purple,
    Green,
    White
  };
  enum class Special : uint32_t { None, Explosive, Lightning, HyperCube };

  Gem() = default;
  Gem(Type type_, Special special_ = Special::None)
      : type(type_), special(special_) {}

  Type type;
  Special special{Special::None};
};

class MatchArea {
 public:
  MatchArea(cen::irect area, std::mt19937 gen);

  void handle_events(cen::event &event);
  void render(cen::renderer &renderer) const;

  cen::irect area() const { return _area; }
  void area(cen::irect area) { _update_area(area); }

 private:
  void _update_area(cen::irect area);

  cen::irect _area;
  std::mt19937 _gen;
  std::vector<Gem> _board;

  static constexpr uint32_t grid_rows = 8;
  static constexpr uint32_t grid_cols = 8;
};

}  // namespace match_idle

#endif