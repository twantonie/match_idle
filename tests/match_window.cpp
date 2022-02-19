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
    CHECK(matches[0].indices.size() == 3);

    grid = {1, 3};
    matches = mi::find_matches(board, grid);
    CHECK(matches.size() == 1);
    CHECK(matches[0].indices.size() == 3);
  }

  SECTION("4 of a kind") {
    std::vector<mi::Gem> board{
        {Type::Blue}, {Type::Blue}, {Type::Blue}, {Type::Blue}};

    mi::GridLayout grid{4, 1};

    auto matches = mi::find_matches(board, grid);
    CHECK(matches.size() == 1);
    CHECK(matches[0].special == mi::Gem::Special::Explosive);
    CHECK(matches[0].indices.size() == 4);

    grid = {1, 4};
    matches = mi::find_matches(board, grid);
    CHECK(matches.size() == 1);
    CHECK(matches[0].special == mi::Gem::Special::Explosive);
    CHECK(matches[0].indices.size() == 4);
  }

  SECTION("L/T") {
    std::vector<mi::Gem> board{{Type::Blue}, {Type::Blue},  {Type::Blue},
                               {Type::Blue}, {Type::Green}, {Type::Green},
                               {Type::Blue}, {Type::White}, {Type::White}};

    mi::GridLayout grid{3, 3};

    auto matches = mi::find_matches(board, grid);

    CHECK(matches.size() == 1);
    CHECK(matches[0].special == mi::Gem::Special::Lightning);
    CHECK(matches[0].indices.size() == 5);

    board = {{Type::Blue},  {Type::Blue}, {Type::Blue},
             {Type::Green}, {Type::Blue}, {Type::Green},
             {Type::White}, {Type::Blue}, {Type::White}};

    matches = mi::find_matches(board, grid);

    CHECK(matches.size() == 1);
    CHECK(matches[0].special == mi::Gem::Special::Lightning);
    CHECK(matches[0].indices.size() == 5);
  }

  SECTION("L/T + 4") {
    // clang-format off
    std::vector<mi::Gem> board{{Type::Blue}, {Type::Blue},  {Type::Blue},  {Type::Blue},  
                               {Type::Blue}, {Type::Green}, {Type::Green}, {Type::White}, 
                               {Type::Blue}, {Type::White}, {Type::White}, {Type::Green}};
    // clang-format on

    mi::GridLayout grid{3, 4};

    auto matches = mi::find_matches(board, grid);
    // XXX: What do we do here? Bejeweled returns both an explosive and
    // lightning gem

    CHECK(matches.size() == 1);
    CHECK(matches[0].special == mi::Gem::Special::Lightning);
    CHECK(matches[0].indices.size() == 6);
  }

  SECTION("5 of a kind") {
    std::vector<mi::Gem> board{
        {Type::Blue}, {Type::Blue}, {Type::Blue}, {Type::Blue}, {Type::Blue}};

    mi::GridLayout grid{5, 1};

    auto matches = mi::find_matches(board, grid);
    CHECK(matches.size() == 1);
    CHECK(matches[0].special == mi::Gem::Special::HyperCube);
    CHECK(matches[0].indices.size() == 5);

    grid = {1, 5};
    matches = mi::find_matches(board, grid);
    CHECK(matches.size() == 1);
    CHECK(matches[0].special == mi::Gem::Special::HyperCube);
    CHECK(matches[0].indices.size() == 5);
  }

  SECTION("6 of a kind") {
    std::vector<mi::Gem> board{{Type::Blue}, {Type::Blue}, {Type::Blue},
                               {Type::Blue}, {Type::Blue}, {Type::Blue}};

    mi::GridLayout grid{6, 1};

    auto matches = mi::find_matches(board, grid);
    CHECK(matches.size() == 1);
    CHECK(matches[0].special == mi::Gem::Special::HyperCube);
    CHECK(matches[0].indices.size() == 6);

    grid = {1, 6};
    matches = mi::find_matches(board, grid);
    CHECK(matches.size() == 1);
    CHECK(matches[0].special == mi::Gem::Special::HyperCube);
    CHECK(matches[0].indices.size() == 6);
  }
}

TEST_CASE("Remove matches") {
  using Type = mi::Gem::Type;

  SECTION("Drop") {
    std::vector<mi::Gem> board{
        {Type::Blue}, {Type::Blue}, {Type::Green}, {Type::Yellow}};

    mi::GridLayout grid{2, 2};

    std::vector<mi::Match> matches{mi::Match{{2, 3}}};

    std::vector<mi::Gem> expected_board{
        {Type::Empty}, {Type::Empty}, {Type::Blue}, {Type::Blue}};

    mi::remove_matches(board, grid, matches);

    CHECK(board == expected_board);
  }

  SECTION("No drop") {
    std::vector<mi::Gem> board{
        {Type::Blue}, {Type::Blue}, {Type::Green}, {Type::Yellow}};

    mi::GridLayout grid{2, 2};

    std::vector<mi::Match> matches{mi::Match{{0, 1}}};

    std::vector<mi::Gem> expected_board{
        {Type::Empty}, {Type::Empty}, {Type::Green}, {Type::Yellow}};

    mi::remove_matches(board, grid, matches);

    CHECK(board == expected_board);
  }

  SECTION("Vertical match") {
    std::vector<mi::Gem> board{
        {Type::Blue}, {Type::Blue}, {Type::Green}, {Type::Yellow}};

    mi::GridLayout grid{2, 2};

    std::vector<mi::Match> matches{mi::Match{{0, 2}}};

    std::vector<mi::Gem> expected_board{
        {Type::Empty}, {Type::Blue}, {Type::Empty}, {Type::Yellow}};

    mi::remove_matches(board, grid, matches);

    CHECK(board == expected_board);
  }

  SECTION("Multiple drops") {
    std::vector<mi::Gem> board{
        {Type::White}, {Type::Blue}, {Type::Green}, {Type::Yellow}};

    mi::GridLayout grid{4, 1};

    std::vector<mi::Match> matches{mi::Match{{1}}, mi::Match{{3}}};

    std::vector<mi::Gem> expected_board{
        {Type::Empty}, {Type::Empty}, {Type::White}, {Type::Green}};

    mi::remove_matches(board, grid, matches);

    CHECK(board == expected_board);
  }
}

TEST_CASE("Possible matches") {
  using Type = mi::Gem::Type;

  std::vector<mi::Gem> board{{Type::Green},  {Type::Blue},   {Type::Blue},
                             {Type::Blue},   {Type::Green},  {Type::Yellow},
                             {Type::Yellow}, {Type::Yellow}, {Type::Green}};

  mi::GridLayout grid{3, 3};

  auto possible_matches = mi::find_possible_matches(board, grid);

  CHECK(possible_matches.size() == 2);

  CHECK(possible_matches[0].pos == cen::ipoint{0, 0});
  CHECK(possible_matches[0].move_dir == mi::MoveDir::Down);
  CHECK(possible_matches[0].matches[0].type == Type::Blue);

  CHECK(possible_matches[1].pos == cen::ipoint{2, 1});
  CHECK(possible_matches[1].move_dir == mi::MoveDir::Down);
  CHECK(possible_matches[1].matches[0].type == Type::Yellow);
}