#ifndef MATCH_IDLE_PIECE_HPP
#define MATCH_IDLE_PIECE_HPP

#include <iostream>
#include <random>

namespace match_idle {

struct Piece {
  static std::uniform_int_distribution<uint32_t> dist;

  enum class Type : uint32_t {
    Red,
    Blue,
    Yellow,
    Orange,
    Purple,
    Green,
    White,
    Empty,
    Copper,
    Silver,
    Gold,
    Sack,
    GreenChest,
    RedChest,
    Vault,
  };

  enum class Special : uint32_t { None, Explosive, Lightning, HyperCube };

  Piece() = default;
  Piece(Type type_, Special special_ = Special::None)
      : type(type_), special(special_) {}
  explicit Piece(std::mt19937 &gen) : type(static_cast<Type>(dist(gen))) {}

  bool operator==(const Piece &rhs) const {
    return type == rhs.type && special == rhs.special;
  }

  friend std::ostream &operator<<(std::ostream &stream, const Piece &gem) {
    return stream << static_cast<uint32_t>(gem.type);
  }

  bool is_movable() const;
  bool is_matchable() const;

  Type type;
  Special special{Special::None};
};

}  // namespace match_idle

#endif