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

  SECTION("4 of a kind") {
    std::vector<mi::Gem> board{
        {Type::Blue}, {Type::Blue}, {Type::Blue}, {Type::Blue}};

    mi::GridLayout grid{4, 1};

    auto matches = mi::find_matches(board, grid);
    CHECK(matches.size() == 1);
    CHECK(matches[0].special == mi::Gem::Special::Explosive);

    grid = {1, 4};
    matches = mi::find_matches(board, grid);
    CHECK(matches.size() == 1);
    CHECK(matches[0].special == mi::Gem::Special::Explosive);
  }

  SECTION("L/T") {
    std::vector<mi::Gem> board{{Type::Blue}, {Type::Blue},  {Type::Blue},
                               {Type::Blue}, {Type::Green}, {Type::Green},
                               {Type::Blue}, {Type::White}, {Type::White}};

    mi::GridLayout grid{3, 3};

    auto matches = mi::find_matches(board, grid);

    CHECK(matches.size() == 1);
    // XXX: What do we do here? Bejeweled returns both an explosive and
    // lightning gem
    CHECK(matches[0].special == mi::Gem::Special::Lightning);

    board = {{Type::Blue},  {Type::Blue}, {Type::Blue},
             {Type::Green}, {Type::Blue}, {Type::Green},
             {Type::White}, {Type::Blue}, {Type::White}};

    matches = mi::find_matches(board, grid);

    CHECK(matches.size() == 1);
    CHECK(matches[0].special == mi::Gem::Special::Lightning);
  }

  SECTION("L/T + 4") {
    std::vector<mi::Gem> board{{Type::Blue},  {Type::Blue},  {Type::Blue},
                               {Type::Blue},  {Type::Blue},  {Type::Green},
                               {Type::Green}, {Type::White}, {Type::Blue},
                               {Type::White}, {Type::White}, {Type::White}};

    mi::GridLayout grid{4, 3};

    auto matches = mi::find_matches(board, grid);

    CHECK(matches.size() == 1);
    CHECK(matches[0].special == mi::Gem::Special::Lightning);
  }

  SECTION("5 of a kind") {
    std::vector<mi::Gem> board{
        {Type::Blue}, {Type::Blue}, {Type::Blue}, {Type::Blue}, {Type::Blue}};

    mi::GridLayout grid{5, 1};

    auto matches = mi::find_matches(board, grid);
    CHECK(matches.size() == 1);
    CHECK(matches[0].special == mi::Gem::Special::HyperCube);

    grid = {1, 5};
    matches = mi::find_matches(board, grid);
    CHECK(matches.size() == 1);
    CHECK(matches[0].special == mi::Gem::Special::HyperCube);
  }

  SECTION("6 of a kind") {
    std::vector<mi::Gem> board{{Type::Blue}, {Type::Blue}, {Type::Blue},
                               {Type::Blue}, {Type::Blue}, {Type::Blue}};

    mi::GridLayout grid{6, 1};

    auto matches = mi::find_matches(board, grid);
    CHECK(matches.size() == 1);
    CHECK(matches[0].special == mi::Gem::Special::HyperCube);

    grid = {1, 6};
    matches = mi::find_matches(board, grid);
    CHECK(matches.size() == 1);
    CHECK(matches[0].special == mi::Gem::Special::HyperCube);
  }
}