#include <opencv2/core/mat.hpp>
#include <string_view>

#include "board.hpp"

namespace cheat {

cv::Mat take_screenshot();
cv::Rect screen_position(char const *window_name);

std::vector<match_idle::Piece> read_board(cv::Mat const &board);

}  // namespace cheat
