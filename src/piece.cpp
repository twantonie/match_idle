#include "piece.hpp"

namespace match_idle {

bool Piece::is_movable() const {
  return type != Type::Empty && type != Type::Vault;
}

bool Piece::is_matchable() const {
  return type != Type::Empty && type != Type::Vault;
}

const char *to_string(Piece::Type type) {
  using T = Piece::Type;
  switch (type) {
    case T::Red:
      return "Red";
    case T::Blue:
      return "Blue";
    case T::Yellow:
      return "Yellow";
    case T::Orange:
      return "Orange";
    case T::Purple:
      return "Purple";
    case T::Green:
      return "Green";
    case T::White:
      return "White";
    case T::Empty:
      return "Empty";
    case T::Copper:
      return "Copper";
    case T::Silver:
      return "Silver";
    case T::Gold:
      return "Gold";
    case T::Sack:
      return "Sack";
    case T::BrownChest:
      return "BrownChest";
    case T::GreenChest:
      return "GreenChest";
    case T::RedChest:
      return "RedChest";
    case T::Vault:
      return "Vault";
  }

  return "";
}

}  // namespace match_idle