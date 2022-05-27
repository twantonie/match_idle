#include <opencv2/core/mat.hpp>
#include <string_view>

#include "board.hpp"

namespace cheat {

cv::Mat take_screenshot();
cv::Rect screen_position(char const *window_name);

std::vector<match_idle::Piece> read_board(cv::Mat const &board);

struct Key {
  int id;
  uint32_t key;
};

void register_key(Key key);
int get_hotkey_id();

}  // namespace cheat
