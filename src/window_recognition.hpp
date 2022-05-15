#include <opencv2/core/mat.hpp>
#include <string_view>

#include "board.hpp"

namespace cheat {

cv::Mat take_screenshot();
cv::Mat resize_to_screen(cv::Mat const &screenshot, char const *window_name);

std::vector<match_idle::Piece> read_board(cv::Mat const &board);

}  // namespace cheat
