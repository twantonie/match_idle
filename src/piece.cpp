#include "piece.hpp"

namespace match_idle {

bool Piece::is_movable() const {
  return type != Type::Empty && type != Type::Vault;
}

bool Piece::is_matchable() const {
  return type != Type::Empty && type != Type::Vault;
}

}  // namespace match_idle