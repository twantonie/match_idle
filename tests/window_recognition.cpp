#include <fmt/core.h>

#include <catch2/catch.hpp>
#include <opencv2/imgcodecs.hpp>
#include <window_recognition.hpp>

namespace mi = match_idle;
using T = mi::Piece::Type;

// clang-format off
std::array<std::vector<mi::Piece>, 8> boards{
    std::vector<mi::Piece>{},
    std::vector<mi::Piece>{},
    std::vector<mi::Piece>{},
    std::vector<mi::Piece>{},
    std::vector<mi::Piece>{
        {T::Sack}, {T::Sack}, {T::Gold}, {T::Gold}, {T::Sack}, {T::Copper}, {T::Gold}, {T::Copper},
        {T::Sack}, {T::Sack}, {T::Silver}, {T::Copper}, {T::Sack}, {T::Silver}, {T::Copper}, {T::Copper},
        {T::Silver}, {T::Gold}, {T::Copper}, {T::Sack}, {T::Copper}, {T::Gold}, {T::Silver}, {T::Gold},
        {T::Sack}, {T::Sack}, {T::Gold}, {T::Silver}, {T::Silver}, {T::Gold}, {T::Silver}, {T::Gold},
        {T::Sack}, {T::Silver}, {T::Copper}, {T::Sack}, {T::Silver}, {T::Silver}, {T::Gold}, {T::Silver},
        {T::Silver}, {T::Copper}, {T::Silver}, {T::Gold}, {T::Gold}, {T::Copper}, {T::Gold}, {T::Silver},
        {T::Copper}, {T::Gold}, {T::Gold}, {T::Silver}, {T::Copper}, {T::Silver}, {T::Sack}, {T::Sack},
        {T::Sack}, {T::Sack}, {T::Silver}, {T::Sack}, {T::Sack}, {T::Gold}, {T::Copper}, {T::Silver}
    },
    std::vector<mi::Piece>{
        {T::Sack}, {T::Gold}, {T::Sack}, {T::BrownChest}, {T::Silver}, {T::Gold}, {T::Sack}, {T::Gold},
        {T::Silver}, {T::Gold}, {T::Copper}, {T::Sack}, {T::Sack}, {T::Silver}, {T::Silver}, {T::Copper},
        {T::Silver}, {T::Sack}, {T::Gold}, {T::Sack}, {T::Copper}, {T::BrownChest}, {T::Gold}, {T::Gold},
        {T::Sack}, {T::Sack}, {T::Copper}, {T::Silver}, {T::Silver}, {T::Sack}, {T::BrownChest}, {T::Silver},
        {T::Silver}, {T::Gold}, {T::BrownChest}, {T::Silver}, {T::Sack}, {T::BrownChest}, {T::Silver}, {T::Sack},
        {T::Sack}, {T::Sack}, {T::Gold}, {T::BrownChest}, {T::GreenChest}, {T::Copper}, {T::Gold}, {T::Copper},
        {T::Copper}, {T::Sack}, {T::Silver}, {T::GreenChest}, {T::RedChest}, {T::Silver}, {T::Gold}, {T::Gold},
        {T::Copper}, {T::RedChest}, {T::BrownChest}, {T::Copper}, {T::Silver}, {T::BrownChest}, {T::Sack}, {T::Gold}
    },
    std::vector<mi::Piece>{},
    std::vector<mi::Piece>{},
};
// clang-format on

std::array<const char*, 8> file_names{
    "treasure_hunt1.png", "treasure_hunt2.png", "treasure_hunt3.png",
    "treasure_hunt4.png", "treasure_hunt5.png", "treasure_hunt6.png",
    "treasure_hunt7.png", "treasure_hunt8.png"};

TEST_CASE("read board") {
  std::string file_location{FILE_DIR};
  size_t loc = GENERATE(range(size_t{0}, boards.size()));
  file_location += "/";
  file_location += file_names[loc];

  auto window = cv::imread(file_location);
  auto board = cheat::read_board(window);

  if (!boards[loc].empty()) CHECK(board == boards[loc]);
}