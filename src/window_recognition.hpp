#include <opencv2/core/mat.hpp>
#include <string_view>

#include "board.hpp"

namespace cheat {

cv::Mat take_screenshot();
cv::Rect screen_position(char const *window_name);
cv::Rect board_position();

std::vector<match_idle::Piece> read_board(cv::Mat const &board);
void move_piece(match_idle::MoveDir move_dir, match_idle::Point pos,
                const cv::Rect screen_pos);

struct Key {
  int id;
  uint32_t key;
};

void register_key(Key key);
int get_hotkey_id();

}  // namespace cheat
