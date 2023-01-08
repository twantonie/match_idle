#include <fmt/core.h>

#include <boost/filesystem.hpp>
#include <catch2/catch.hpp>
#include <opencv2/imgcodecs.hpp>
#include <window_recognition.hpp>

namespace mi = match_idle;
using Type = mi::Piece::Type;
using Board = mi::Board;

constexpr std::array file_names{
    "treasure_hunt_1.png",  "treasure_hunt_2.png",  "treasure_hunt_3.png",
    "treasure_hunt_4.png",  "treasure_hunt_5.png",  "treasure_hunt_6.png",
    "treasure_hunt_7.png",  "treasure_hunt_8.png",  "treasure_hunt_9.png",
    "treasure_hunt_10.png", "treasure_hunt_11.png", "treasure_hunt_12.png",
    "treasure_hunt_13.png", "treasure_hunt_14.png"};

// clang-format off
std::array<std::vector<mi::Piece>, file_names.size()> boards{
Board{
  {Type::Sack}, {Type::Sack}, {Type::Gold}, {Type::Sack}, {Type::Gold}, {Type::Sack}, {Type::Gold}, {Type::Gold},
  {Type::Copper}, {Type::Gold}, {Type::Copper}, {Type::Gold}, {Type::Gold}, {Type::Silver}, {Type::Sack}, {Type::Copper},
  {Type::Silver}, {Type::Silver}, {Type::Sack}, {Type::Copper}, {Type::Silver}, {Type::Copper}, {Type::Copper}, {Type::Sack},
  {Type::Sack}, {Type::Copper}, {Type::Copper}, {Type::Silver}, {Type::Copper}, {Type::Gold}, {Type::Gold}, {Type::Copper},
  {Type::Gold}, {Type::Sack}, {Type::Sack}, {Type::Gold}, {Type::Copper}, {Type::Sack}, {Type::Silver}, {Type::Gold},
  {Type::Copper}, {Type::Sack}, {Type::Gold}, {Type::Sack}, {Type::Sack}, {Type::Copper}, {Type::Copper}, {Type::Gold},
  {Type::Silver}, {Type::Gold}, {Type::Copper}, {Type::Silver}, {Type::Silver}, {Type::Gold}, {Type::Silver}, {Type::Copper},
  {Type::Copper}, {Type::Silver}, {Type::Silver}, {Type::Gold}, {Type::Gold}, {Type::Sack}, {Type::Copper}, {Type::Sack},
},
Board{
  {Type::Sack}, {Type::Sack}, {Type::Gold}, {Type::Sack}, {Type::Sack}, {Type::Copper}, {Type::Sack}, {Type::Gold},
  {Type::Copper}, {Type::Sack}, {Type::Silver}, {Type::BrownChest}, {Type::Copper}, {Type::Sack}, {Type::Sack}, {Type::Copper},
  {Type::Silver}, {Type::Gold}, {Type::Copper}, {Type::Gold}, {Type::Copper}, {Type::BrownChest}, {Type::Copper}, {Type::Sack},
  {Type::Sack}, {Type::Silver}, {Type::Copper}, {Type::Sack}, {Type::Gold}, {Type::Sack}, {Type::BrownChest}, {Type::Copper},
  {Type::Gold}, {Type::Copper}, {Type::Sack}, {Type::Gold}, {Type::Sack}, {Type::BrownChest}, {Type::Silver}, {Type::Gold},
  {Type::Copper}, {Type::Sack}, {Type::Copper}, {Type::Silver}, {Type::Gold}, {Type::Copper}, {Type::Copper}, {Type::Gold},
  {Type::Silver}, {Type::Gold}, {Type::Gold}, {Type::BrownChest}, {Type::Sack}, {Type::Gold}, {Type::Silver}, {Type::Copper},
  {Type::Copper}, {Type::Silver}, {Type::Copper}, {Type::Sack}, {Type::Silver}, {Type::Sack}, {Type::Copper}, {Type::Sack},
},
Board{
  {Type::Silver}, {Type::Gold}, {Type::Copper}, {Type::Silver}, {Type::Silver}, {Type::Copper}, {Type::Sack}, {Type::Copper},
  {Type::Sack}, {Type::Gold}, {Type::Copper}, {Type::Gold}, {Type::Silver}, {Type::Sack}, {Type::Sack}, {Type::Copper},
  {Type::Silver}, {Type::Copper}, {Type::BrownChest}, {Type::Copper}, {Type::GreenChest}, {Type::Copper}, {Type::Copper}, {Type::Sack},
  {Type::Silver}, {Type::Gold}, {Type::Copper}, {Type::Silver}, {Type::Silver}, {Type::BrownChest}, {Type::BrownChest}, {Type::Copper},
  {Type::Gold}, {Type::Copper}, {Type::Silver}, {Type::BrownChest}, {Type::Copper}, {Type::BrownChest}, {Type::Silver}, {Type::Gold},
  {Type::Copper}, {Type::BrownChest}, {Type::Silver}, {Type::Silver}, {Type::Sack}, {Type::Copper}, {Type::Copper}, {Type::Gold},
  {Type::Silver}, {Type::Gold}, {Type::Gold}, {Type::BrownChest}, {Type::Sack}, {Type::Gold}, {Type::Silver}, {Type::Copper},
  {Type::Copper}, {Type::Silver}, {Type::Copper}, {Type::Sack}, {Type::Silver}, {Type::Sack}, {Type::Copper}, {Type::Sack},
},
Board{
  {Type::Sack}, {Type::Gold}, {Type::Silver}, {Type::Sack}, {Type::Sack}, {Type::Gold}, {Type::Gold}, {Type::Copper},
  {Type::Copper}, {Type::Sack}, {Type::Gold}, {Type::Sack}, {Type::Silver}, {Type::Sack}, {Type::Copper}, {Type::Copper},
  {Type::Silver}, {Type::Gold}, {Type::Sack}, {Type::Gold}, {Type::Silver}, {Type::Silver}, {Type::BrownChest}, {Type::Silver},
  {Type::Silver}, {Type::BrownChest}, {Type::Silver}, {Type::Copper}, {Type::BrownChest}, {Type::BrownChest}, {Type::Gold}, {Type::Silver},
  {Type::Gold}, {Type::Copper}, {Type::Silver}, {Type::Sack}, {Type::Copper}, {Type::Sack}, {Type::Silver}, {Type::Gold},
  {Type::Copper}, {Type::BrownChest}, {Type::Copper}, {Type::Silver}, {Type::RedChest}, {Type::Copper}, {Type::Sack}, {Type::Gold},
  {Type::Silver}, {Type::Gold}, {Type::Gold}, {Type::Silver}, {Type::Sack}, {Type::Gold}, {Type::Gold}, {Type::Copper},
  {Type::Copper}, {Type::Silver}, {Type::Copper}, {Type::GreenChest}, {Type::Silver}, {Type::Sack}, {Type::Copper}, {Type::Sack},
},
Board{
  {Type::Silver}, {Type::Silver}, {Type::BrownChest}, {Type::Silver}, {Type::Gold}, {Type::Gold}, {Type::Silver}, {Type::Copper},
  {Type::Sack}, {Type::Copper}, {Type::Silver}, {Type::Copper}, {Type::Sack}, {Type::Copper}, {Type::BrownChest}, {Type::Copper},
  {Type::Copper}, {Type::Gold}, {Type::BrownChest}, {Type::Gold}, {Type::Gold}, {Type::Sack}, {Type::Gold}, {Type::BrownChest},
  {Type::Silver}, {Type::Gold}, {Type::Copper}, {Type::Copper}, {Type::Gold}, {Type::Gold}, {Type::BrownChest}, {Type::Gold},
  {Type::Gold}, {Type::BrownChest}, {Type::BrownChest}, {Type::Copper}, {Type::Sack}, {Type::Sack}, {Type::GreenChest}, {Type::Copper},
  {Type::Copper}, {Type::Sack}, {Type::BrownChest}, {Type::Sack}, {Type::GreenChest}, {Type::Silver}, {Type::BrownChest}, {Type::Copper},
  {Type::Silver}, {Type::BrownChest}, {Type::Silver}, {Type::Sack}, {Type::RedChest}, {Type::Silver}, {Type::Silver}, {Type::Sack},
  {Type::Copper}, {Type::Silver}, {Type::Silver}, {Type::GreenChest}, {Type::Silver}, {Type::GreenChest}, {Type::Silver}, {Type::Sack},
},
Board{
  {Type::Copper}, {Type::Copper}, {Type::Silver}, {Type::Copper}, {Type::Sack}, {Type::Gold}, {Type::Copper}, {Type::Silver},
  {Type::Silver}, {Type::Silver}, {Type::Gold}, {Type::Copper}, {Type::Silver}, {Type::Copper}, {Type::BrownChest}, {Type::Copper},
  {Type::Sack}, {Type::BrownChest}, {Type::Sack}, {Type::Silver}, {Type::BrownChest}, {Type::Silver}, {Type::Silver}, {Type::Sack},
  {Type::Sack}, {Type::Copper}, {Type::BrownChest}, {Type::Gold}, {Type::Gold}, {Type::BrownChest}, {Type::Gold}, {Type::Silver},
  {Type::Silver}, {Type::Copper}, {Type::Sack}, {Type::Sack}, {Type::Silver}, {Type::Copper}, {Type::BrownChest}, {Type::Copper},
  {Type::Gold}, {Type::BrownChest}, {Type::BrownChest}, {Type::Silver}, {Type::BrownChest}, {Type::Silver}, {Type::Copper}, {Type::Copper},
  {Type::Sack}, {Type::Copper}, {Type::GreenChest}, {Type::Gold}, {Type::Gold}, {Type::Sack}, {Type::GreenChest}, {Type::Gold},
  {Type::Gold}, {Type::Copper}, {Type::Silver}, {Type::BrownChest}, {Type::GreenChest}, {Type::BrownChest}, {Type::Sack}, {Type::Copper},
},
Board{
  {Type::Copper}, {Type::Gold}, {Type::Sack}, {Type::Gold}, {Type::Gold}, {Type::Silver}, {Type::Sack}, {Type::Gold},
  {Type::Copper}, {Type::Silver}, {Type::BrownChest}, {Type::Copper}, {Type::Silver}, {Type::Gold}, {Type::Sack}, {Type::Gold},
  {Type::Sack}, {Type::Gold}, {Type::Copper}, {Type::Silver}, {Type::BrownChest}, {Type::Gold}, {Type::Silver}, {Type::Copper},
  {Type::Silver}, {Type::Silver}, {Type::BrownChest}, {Type::Gold}, {Type::Copper}, {Type::Sack}, {Type::Silver}, {Type::Silver},
  {Type::Sack}, {Type::Gold}, {Type::GreenChest}, {Type::Silver}, {Type::RedChest}, {Type::Copper}, {Type::Gold}, {Type::Gold},
  {Type::Copper}, {Type::BrownChest}, {Type::RedChest}, {Type::GreenChest}, {Type::Gold}, {Type::GreenChest}, {Type::GreenChest}, {Type::Sack},
  {Type::BrownChest}, {Type::Copper}, {Type::Gold}, {Type::BrownChest}, {Type::Copper}, {Type::Sack}, {Type::RedChest}, {Type::Silver},
  {Type::Gold}, {Type::Silver}, {Type::RedChest}, {Type::BrownChest}, {Type::GreenChest}, {Type::BrownChest}, {Type::Sack}, {Type::Gold},
},
Board{
  {Type::Gold}, {Type::Sack}, {Type::Sack}, {Type::Gold}, {Type::Silver}, {Type::Silver}, {Type::Sack}, {Type::Silver},
  {Type::Silver}, {Type::Copper}, {Type::Sack}, {Type::Silver}, {Type::Sack}, {Type::Copper}, {Type::Sack}, {Type::Silver},
  {Type::Sack}, {Type::Gold}, {Type::BrownChest}, {Type::Gold}, {Type::Silver}, {Type::Gold}, {Type::BrownChest}, {Type::Gold},
  {Type::Sack}, {Type::Sack}, {Type::Gold}, {Type::RedChest}, {Type::BrownChest}, {Type::Silver}, {Type::Copper}, {Type::Sack},
  {Type::Gold}, {Type::BrownChest}, {Type::BrownChest}, {Type::Copper}, {Type::Gold}, {Type::Sack}, {Type::BrownChest}, {Type::RedChest},
  {Type::Copper}, {Type::Copper}, {Type::Vault}, {Type::Sack}, {Type::Sack}, {Type::RedChest}, {Type::Copper}, {Type::BrownChest},
  {Type::RedChest}, {Type::Gold}, {Type::BrownChest}, {Type::Sack}, {Type::Copper}, {Type::Copper}, {Type::GreenChest}, {Type::Silver},
  {Type::Gold}, {Type::Silver}, {Type::GreenChest}, {Type::Silver}, {Type::GreenChest}, {Type::Sack}, {Type::Sack}, {Type::RedChest},
},
Board{
  {Type::Silver}, {Type::Sack}, {Type::Silver}, {Type::Silver}, {Type::Copper}, {Type::Gold}, {Type::Silver}, {Type::Gold},
  {Type::Gold}, {Type::Gold}, {Type::Copper}, {Type::Copper}, {Type::Gold}, {Type::Gold}, {Type::BrownChest}, {Type::Gold},
  {Type::Copper}, {Type::Sack}, {Type::BrownChest}, {Type::Sack}, {Type::Gold}, {Type::Sack}, {Type::Silver}, {Type::Copper},
  {Type::Silver}, {Type::Gold}, {Type::Silver}, {Type::Gold}, {Type::Silver}, {Type::Sack}, {Type::Copper}, {Type::Silver},
  {Type::Silver}, {Type::BrownChest}, {Type::Sack}, {Type::Silver}, {Type::Sack}, {Type::Copper}, {Type::Sack}, {Type::Gold},
  {Type::Copper}, {Type::Copper}, {Type::BrownChest}, {Type::Sack}, {Type::BrownChest}, {Type::BrownChest}, {Type::Silver}, {Type::Silver},
  {Type::Gold}, {Type::GreenChest}, {Type::GreenChest}, {Type::Silver}, {Type::Sack}, {Type::BrownChest}, {Type::Sack}, {Type::Sack},
  {Type::Sack}, {Type::Copper}, {Type::BrownChest}, {Type::Sack}, {Type::BrownChest}, {Type::Silver}, {Type::Copper}, {Type::Gold},
},
Board{
  {Type::Sack}, {Type::Gold}, {Type::Silver}, {Type::Gold}, {Type::Copper}, {Type::Sack}, {Type::Sack}, {Type::Copper},
  {Type::Gold}, {Type::Copper}, {Type::Silver}, {Type::Sack}, {Type::Sack}, {Type::Copper}, {Type::Copper}, {Type::Sack},
  {Type::Sack}, {Type::Sack}, {Type::Copper}, {Type::Sack}, {Type::Silver}, {Type::BrownChest}, {Type::Gold}, {Type::Gold},
  {Type::BrownChest}, {Type::Gold}, {Type::BrownChest}, {Type::BrownChest}, {Type::Gold}, {Type::BrownChest}, {Type::Silver}, {Type::Sack},
  {Type::Silver}, {Type::Silver}, {Type::Gold}, {Type::Silver}, {Type::Gold}, {Type::GreenChest}, {Type::Gold}, {Type::Copper},
  {Type::Silver}, {Type::Sack}, {Type::RedChest}, {Type::BrownChest}, {Type::BrownChest}, {Type::RedChest}, {Type::Silver}, {Type::Gold},
  {Type::Copper}, {Type::GreenChest}, {Type::Sack}, {Type::GreenChest}, {Type::Silver}, {Type::Sack}, {Type::Sack}, {Type::Copper},
  {Type::Copper}, {Type::Silver}, {Type::Silver}, {Type::Gold}, {Type::Sack}, {Type::Silver}, {Type::GreenChest}, {Type::Gold},
},
Board{
  {Type::Copper}, {Type::Copper}, {Type::Gold}, {Type::Gold}, {Type::Silver}, {Type::Sack}, {Type::Silver}, {Type::Silver}, 
  {Type::Copper}, {Type::Silver}, {Type::Silver}, {Type::Copper}, {Type::Sack}, {Type::Copper}, {Type::Silver}, {Type::Copper},       
  {Type::BrownChest}, {Type::Gold}, {Type::BrownChest}, {Type::BrownChest}, {Type::Sack}, {Type::Sack}, {Type::Gold}, {Type::Copper}, 
  {Type::BrownChest}, {Type::GreenChest}, {Type::GreenChest}, {Type::Sack}, {Type::Copper}, {Type::Sack}, {Type::Gold}, {Type::Gold}, 
  {Type::Gold}, {Type::Copper}, {Type::Sack}, {Type::Sack}, {Type::RedChest}, {Type::Copper}, {Type::GreenChest}, {Type::Silver},
  {Type::Copper}, {Type::Silver}, {Type::Sack}, {Type::RedChest}, {Type::Silver}, {Type::Sack}, {Type::Silver}, {Type::Silver},
  {Type::Sack}, {Type::GreenChest}, {Type::Silver}, {Type::Copper}, {Type::Sack}, {Type::GreenChest}, {Type::Gold}, {Type::Sack},
  {Type::Sack}, {Type::Gold}, {Type::GreenChest}, {Type::Silver}, {Type::Copper}, {Type::Vault}, {Type::Copper}, {Type::GreenChest},
},
Board{
  {Type::Silver}, {Type::Gold}, {Type::Sack}, {Type::Copper}, {Type::Sack}, {Type::Sack}, {Type::Copper}, {Type::Gold},
  {Type::Sack}, {Type::Gold}, {Type::Copper}, {Type::Silver}, {Type::Copper}, {Type::Gold}, {Type::Copper}, {Type::Silver},
  {Type::Copper}, {Type::Silver}, {Type::Sack}, {Type::Sack}, {Type::Copper}, {Type::Copper}, {Type::BrownChest}, {Type::Sack},
  {Type::Silver}, {Type::Sack}, {Type::Silver}, {Type::BrownChest}, {Type::Gold}, {Type::Sack}, {Type::Silver}, {Type::Copper},
  {Type::BrownChest}, {Type::Gold}, {Type::Sack}, {Type::Silver}, {Type::Copper}, {Type::BrownChest}, {Type::BrownChest}, {Type::Sack},
  {Type::Gold}, {Type::Copper}, {Type::Gold}, {Type::Sack}, {Type::Sack}, {Type::BrownChest}, {Type::GreenChest}, {Type::Copper},
  {Type::BrownChest}, {Type::Copper}, {Type::Silver}, {Type::GreenChest}, {Type::Sack}, {Type::Silver}, {Type::Gold}, {Type::BrownChest},
  {Type::Silver}, {Type::RedChest}, {Type::GreenChest}, {Type::BrownChest}, {Type::RedChest}, {Type::Copper}, {Type::Copper}, {Type::Sack},
},
Board{
  {Type::Silver}, {Type::Copper}, {Type::Gold}, {Type::Copper}, {Type::Silver}, {Type::Silver}, {Type::Gold}, {Type::Copper},
  {Type::Gold}, {Type::Sack}, {Type::Silver}, {Type::Sack}, {Type::Gold}, {Type::Sack}, {Type::Silver}, {Type::Gold},
  {Type::Gold}, {Type::Gold}, {Type::Sack}, {Type::Gold}, {Type::Gold}, {Type::Sack}, {Type::Sack}, {Type::Copper},
  {Type::Sack}, {Type::Copper}, {Type::Copper}, {Type::Gold}, {Type::GreenChest}, {Type::Copper}, {Type::BrownChest}, {Type::Silver},
  {Type::Copper}, {Type::Silver}, {Type::RedChest}, {Type::BrownChest}, {Type::Silver}, {Type::Copper}, {Type::Gold}, {Type::Gold},
  {Type::BrownChest}, {Type::RedChest}, {Type::Silver}, {Type::GreenChest}, {Type::RedChest}, {Type::Silver}, {Type::Silver}, {Type::Copper},
  {Type::Gold}, {Type::Silver}, {Type::Copper}, {Type::Vault}, {Type::BrownChest}, {Type::GreenChest}, {Type::Sack}, {Type::GreenChest},
  {Type::GreenChest}, {Type::Gold}, {Type::Sack}, {Type::GreenChest}, {Type::Vault}, {Type::Gold}, {Type::Gold}, {Type::GreenChest},
},
Board{
  {Type::Silver}, {Type::Sack}, {Type::Silver}, {Type::Copper}, {Type::Silver}, {Type::Gold}, {Type::Sack}, {Type::Gold},
  {Type::Gold}, {Type::Gold}, {Type::BrownChest}, {Type::Copper}, {Type::Copper}, {Type::Gold}, {Type::BrownChest}, {Type::Sack},
  {Type::BrownChest}, {Type::Copper}, {Type::BrownChest}, {Type::Gold}, {Type::Silver}, {Type::Sack}, {Type::GreenChest}, {Type::Copper}, 
  {Type::Silver}, {Type::Silver}, {Type::GreenChest}, {Type::GreenChest}, {Type::Copper}, {Type::Copper}, {Type::Gold}, {Type::Silver},   
  {Type::Sack}, {Type::Copper}, {Type::Copper}, {Type::GreenChest}, {Type::Sack}, {Type::BrownChest}, {Type::GreenChest}, {Type::Copper}, 
  {Type::Gold}, {Type::Gold}, {Type::Sack}, {Type::BrownChest}, {Type::Silver}, {Type::Copper}, {Type::BrownChest}, {Type::Gold},
  {Type::Sack}, {Type::BrownChest}, {Type::BrownChest}, {Type::Vault}, {Type::Vault}, {Type::GreenChest}, {Type::RedChest}, {Type::Sack}, 
  {Type::Sack}, {Type::Copper}, {Type::Gold}, {Type::BrownChest}, {Type::Copper}, {Type::Gold}, {Type::Copper}, {Type::BrownChest},      
},
};
// clang-format on

TEST_CASE("read board") {
  std::string file_location{FILE_DIR};
  size_t loc = GENERATE(range(size_t{0}, file_names.size()));
  file_location += "/";
  file_location += file_names[loc];

  if (boost::filesystem::is_regular_file(file_location)) {
    auto window = cv::imread(file_location);
    auto board = cheat::read_board(window);

    if (!boards[loc].empty()) {
      CHECK(board == boards[loc]);
    } else {
      static constexpr mi::GridLayout grid{8, 8};
      fmt::print("{}\n", loc);
      mi::print_board(board, grid);

      auto it = std::find_if(
          board.begin(), board.end(),
          [](const mi::Piece &piece) { return piece.type == Type::Empty; });
      CHECK(it == board.end());
    }
  } else {
    FAIL_CHECK();
  }
}