#include <catch2/catch.hpp>
#include <match_window.hpp>

namespace mi = match_idle;

TEST_CASE("find matches") {
  using Type = mi::Gem::Type;

  SECTION("3 of a kind") {
    std::vector<mi::Gem> board{{Type::Blue}, {Type::Blue}, {Type::Blue}};

    mi::GridLayout grid{3, 1};

    auto matches = mi::find_matches(board, grid);
    CHECK(matches.size() == 1);

    grid = {1, 3};
    matches = mi::find_matches(board, grid);
    CHECK(matches.size() == 1);
  }
}