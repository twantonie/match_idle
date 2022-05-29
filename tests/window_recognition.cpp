#include <fmt/core.h>

#include <catch2/catch.hpp>
#include <opencv2/imgcodecs.hpp>
#include <window_recognition.hpp>

namespace mi = match_idle;
using T = mi::Piece::Type;

constexpr size_t array_size = 10;

// clang-format off
std::array<std::vector<mi::Piece>, 10> boards{
    std::vector<mi::Piece>{
        {T::Gold}, {T::Gold}, {T::Copper}, {T::Silver}, {T::Gold}, {T::Silver}, {T::Gold}, {T::Sack},
        {T::Gold}, {T::BrownChest}, {T::Gold}, {T::Silver}, {T::Silver}, {T::Gold}, {T::Silver}, {T::Gold},
        {T::Sack}, {T::Copper}, {T::Sack}, {T::Sack}, {T::Copper}, {T::Sack}, {T::BrownChest}, {T::Sack},
        {T::Silver}, {T::Sack}, {T::Sack}, {T::Gold}, {T::Sack}, {T::Copper}, {T::Copper}, {T::Gold},
        {T::Copper}, {T::Gold}, {T::Gold}, {T::BrownChest}, {T::Silver}, {T::Sack}, {T::Sack}, {T::BrownChest},
        {T::Copper}, {T::Copper}, {T::Gold}, {T::Gold}, {T::BrownChest}, {T::GreenChest}, {T::GreenChest}, {T::BrownChest},
        {T::Gold}, {T::Sack}, {T::Copper}, {T::Vault}, {T::BrownChest}, {T::Gold}, {T::Silver}, {T::Gold},
        {T::Sack}, {T::Silver}, {T::BrownChest}, {T::Sack}, {T::Silver}, {T::GreenChest}, {T::Gold}, {T::Sack},
    },
    std::vector<mi::Piece>{
        {T::Gold}, {T::Copper}, {T::BrownChest}, {T::Gold}, {T::Silver}, {T::Sack}, {T::Silver}, {T::Gold},
        {T::Sack}, {T::Silver}, {T::Silver}, {T::Gold}, {T::Gold}, {T::Sack}, {T::Copper}, {T::Copper},
        {T::Gold}, {T::BrownChest}, {T::Silver}, {T::Copper}, {T::Gold}, {T::Copper}, {T::Gold}, {T::Silver},
        {T::Copper}, {T::BrownChest}, {T::Gold}, {T::Copper}, {T::BrownChest}, {T::BrownChest}, {T::GreenChest}, {T::Gold},
        {T::Gold}, {T::Sack}, {T::Silver}, {T::Sack}, {T::Silver}, {T::Sack}, {T::Sack}, {T::BrownChest},
        {T::Gold}, {T::Silver}, {T::BrownChest}, {T::Copper}, {T::Sack}, {T::Gold}, {T::BrownChest}, {T::Gold},
        {T::Sack}, {T::Copper}, {T::Silver}, {T::Silver}, {T::Gold}, {T::Copper}, {T::Copper}, {T::Sack},
        {T::Sack}, {T::Silver}, {T::Gold}, {T::Copper}, {T::Sack}, {T::Sack}, {T::Copper}, {T::Copper},
    },
    std::vector<mi::Piece>{
        {T::Copper}, {T::Gold}, {T::Sack}, {T::Copper}, {T::Copper}, {T::Gold}, {T::Sack}, {T::Gold},
        {T::Sack}, {T::Gold}, {T::Silver}, {T::Silver}, {T::Gold}, {T::Copper}, {T::Sack}, {T::Sack},
        {T::Gold}, {T::Silver}, {T::Sack}, {T::Silver}, {T::Silver}, {T::Gold}, {T::Copper}, {T::Gold},
        {T::Sack}, {T::BrownChest}, {T::Gold}, {T::Sack}, {T::Sack}, {T::BrownChest}, {T::Silver}, {T::Gold},
        {T::Copper}, {T::Copper}, {T::BrownChest}, {T::Copper}, {T::Gold}, {T::BrownChest}, {T::Gold}, {T::Silver},
        {T::Sack}, {T::GreenChest}, {T::Gold}, {T::BrownChest}, {T::Silver}, {T::Gold}, {T::Copper}, {T::Sack},
        {T::Gold}, {T::Gold}, {T::Copper}, {T::RedChest}, {T::Copper}, {T::GreenChest}, {T::Copper}, {T::Copper},
        {T::Copper}, {T::BrownChest}, {T::Sack}, {T::Silver}, {T::Vault}, {T::Silver}, {T::BrownChest}, {T::Silver},
    },
    std::vector<mi::Piece>{
        {T::Gold}, {T::Sack}, {T::Silver}, {T::Gold}, {T::Sack}, {T::Silver}, {T::Copper}, {T::Sack},
        {T::Copper}, {T::Gold}, {T::Sack}, {T::Copper}, {T::Gold}, {T::Copper}, {T::Copper}, {T::Gold},
        {T::Sack}, {T::BrownChest}, {T::Silver}, {T::Copper}, {T::Gold}, {T::Sack}, {T::BrownChest}, {T::Sack},
        {T::Copper}, {T::Copper}, {T::Gold}, {T::Silver}, {T::Sack}, {T::Copper}, {T::Sack}, {T::Gold},
        {T::Copper}, {T::BrownChest}, {T::Sack}, {T::Gold}, {T::Copper}, {T::Gold}, {T::Gold}, {T::Copper},
        {T::Sack}, {T::Silver}, {T::Copper}, {T::RedChest}, {T::Silver}, {T::Gold}, {T::GreenChest}, {T::Copper},
        {T::Sack}, {T::GreenChest}, {T::Sack}, {T::RedChest}, {T::GreenChest}, {T::BrownChest}, {T::Copper}, {T::Sack},
        {T::Copper}, {T::BrownChest}, {T::Sack}, {T::Silver}, {T::Vault}, {T::GreenChest}, {T::Sack}, {T::Silver},
    },
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
    std::vector<mi::Piece>{
        {T::Copper}, {T::Gold}, {T::Copper}, {T::Gold}, {T::Silver}, {T::Gold}, {T::Silver}, {T::Sack},
        {T::Gold}, {T::BrownChest}, {T::Gold}, {T::Gold}, {T::Silver}, {T::Gold}, {T::Gold}, {T::Silver},
        {T::Silver}, {T::Copper}, {T::Copper}, {T::Sack}, {T::Sack}, {T::BrownChest}, {T::Silver}, {T::BrownChest},
        {T::Silver}, {T::Gold}, {T::Sack}, {T::Copper}, {T::Silver}, {T::Gold}, {T::Sack}, {T::Silver},
        {T::Copper}, {T::Sack}, {T::Gold}, {T::Gold}, {T::Sack}, {T::Sack}, {T::BrownChest}, {T::Silver},
        {T::GreenChest}, {T::Gold}, {T::Sack}, {T::Sack}, {T::Copper}, {T::GreenChest}, {T::Silver}, {T::Sack},
        {T::Silver}, {T::Gold}, {T::Copper}, {T::Sack}, {T::Gold}, {T::Sack}, {T::Gold}, {T::Sack},
        {T::Sack}, {T::Sack}, {T::RedChest}, {T::Silver}, {T::Sack}, {T::Silver}, {T::Copper}, {T::Gold},
    },
    std::vector<mi::Piece>{
        {T::Copper}, {T::Silver}, {T::Sack}, {T::Copper}, {T::Copper}, {T::Silver}, {T::Sack}, {T::Gold},
        {T::Copper}, {T::Silver}, {T::Copper}, {T::Gold}, {T::Silver}, {T::Silver}, {T::Gold}, {T::Silver},
        {T::Silver}, {T::Sack}, {T::Silver}, {T::Gold}, {T::Sack}, {T::Gold}, {T::Sack}, {T::Sack},
        {T::Gold}, {T::Copper}, {T::Copper}, {T::Silver}, {T::Silver}, {T::Copper}, {T::Sack}, {T::Copper},
        {T::Copper}, {T::Gold}, {T::GreenChest}, {T::Sack}, {T::GreenChest}, {T::Silver}, {T::Silver}, {T::Sack},
        {T::Silver}, {T::BrownChest}, {T::Copper}, {T::BrownChest}, {T::Copper}, {T::GreenChest}, {T::Sack}, {T::Gold},
        {T::Gold}, {T::GreenChest}, {T::Copper}, {T::Silver}, {T::Copper}, {T::Silver}, {T::GreenChest}, {T::Copper},
        {T::BrownChest}, {T::Gold}, {T::RedChest}, {T::BrownChest}, {T::GreenChest}, {T::BrownChest}, {T::Sack}, {T::Silver},
    },
    std::vector<mi::Piece>{
        {T::Copper}, {T::Sack}, {T::Silver}, {T::Copper}, {T::Gold}, {T::Copper}, {T::Silver}, {T::Copper},
        {T::Gold}, {T::Sack}, {T::Copper}, {T::Sack}, {T::Silver}, {T::Sack}, {T::BrownChest}, {T::Gold},
        {T::GreenChest}, {T::Copper}, {T::Silver}, {T::Silver}, {T::Sack}, {T::Copper}, {T::Gold}, {T::Silver},
        {T::Sack}, {T::Copper}, {T::Sack}, {T::Copper}, {T::Gold}, {T::Silver}, {T::BrownChest}, {T::Copper},
        {T::Copper}, {T::Sack}, {T::Copper}, {T::Sack}, {T::Gold}, {T::BrownChest}, {T::Gold}, {T::Silver},
        {T::Sack}, {T::Silver}, {T::Silver}, {T::BrownChest}, {T::Copper}, {T::Copper}, {T::Gold}, {T::Sack},
        {T::Copper}, {T::BrownChest}, {T::Vault}, {T::BrownChest}, {T::Copper}, {T::Gold}, {T::RedChest}, {T::Silver},
        {T::Gold}, {T::Sack}, {T::Copper}, {T::Silver}, {T::GreenChest}, {T::GreenChest}, {T::Gold}, {T::BrownChest},
    },
    std::vector<mi::Piece>{
        {T::Gold}, {T::Sack}, {T::Copper}, {T::Gold}, {T::Silver}, {T::Copper}, {T::Gold}, {T::Sack},
        {T::Sack}, {T::Gold}, {T::Silver}, {T::Gold}, {T::Sack}, {T::Silver}, {T::Sack}, {T::Copper},
        {T::Copper}, {T::BrownChest}, {T::Silver}, {T::BrownChest}, {T::Copper}, {T::Gold}, {T::BrownChest}, {T::Silver},
        {T::BrownChest}, {T::BrownChest}, {T::Copper}, {T::Gold}, {T::BrownChest}, {T::Sack}, {T::Gold}, {T::Copper},
        {T::Copper}, {T::Sack}, {T::GreenChest}, {T::Copper}, {T::Sack}, {T::GreenChest}, {T::GreenChest}, {T::Silver},
        {T::Sack}, {T::GreenChest}, {T::RedChest}, {T::Gold}, {T::Gold}, {T::Sack}, {T::Copper}, {T::Gold},
        {T::Copper}, {T::Silver}, {T::Vault}, {T::GreenChest}, {T::Copper}, {T::Gold}, {T::RedChest}, {T::Silver},
        {T::Gold}, {T::Sack}, {T::Copper}, {T::Gold}, {T::GreenChest}, {T::Vault}, {T::Gold}, {T::BrownChest},
    },
};
// clang-format on

std::array<const char*, 10> file_names{
    "treasure_hunt1.png", "treasure_hunt2.png", "treasure_hunt3.png",
    "treasure_hunt4.png", "treasure_hunt5.png", "treasure_hunt6.png",
    "treasure_hunt7.png", "treasure_hunt8.png", "treasure_hunt9.png",
    "treasure_hunt10.png"};

TEST_CASE("read board") {
  std::string file_location{FILE_DIR};
  size_t loc = GENERATE(range(size_t{0}, array_size));
  file_location += "/";
  file_location += file_names[loc];

  auto window = cv::imread(file_location);
  auto board = cheat::read_board(window);

  if (!boards[loc].empty()) {
    CHECK(board == boards[loc]);
  } else {
    static constexpr mi::GridLayout grid{8, 8};
    fmt::print("{}\n", loc);
    mi::print_board(board, grid);
  }
}