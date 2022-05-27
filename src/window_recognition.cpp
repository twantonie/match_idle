#include "window_recognition.hpp"

#include <fmt/core.h>

#include <stdexcept>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace cheat {

namespace mi = match_idle;

cv::Mat take_screenshot() {
  // Adapted from
  // https://stackoverflow.com/questions/14148758/how-to-capture-the-desktop-in-opencv-ie-turn-a-bitmap-into-a-mat/14167433#14167433
  // on 8/05/2022

  HDC hwindowDC = GetDC(nullptr);
  HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);

  int width = GetDeviceCaps(hwindowDC, HORZRES);
  int height = GetDeviceCaps(hwindowDC, VERTRES);

  cv::Mat src(height, width, CV_8UC4);

  // create a bitmap
  HBITMAP hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);

  BITMAPINFOHEADER bi{};
  // http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
  bi.biSize = sizeof(BITMAPINFOHEADER);
  bi.biWidth = width;
  bi.biHeight = -height;
  bi.biPlanes = 1;
  bi.biBitCount = 32;
  bi.biCompression = BI_RGB;

  // use the previously created device context with the bitmap
  SelectObject(hwindowCompatibleDC, hbwindow);
  // copy from the window device context to the bitmap device context
  BitBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, SRCCOPY);
  // copy from hwindowCompatibleDC to hbwindow
  GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data,
            (BITMAPINFO *)&bi, DIB_RGB_COLORS);

  // avoid memory leak
  DeleteObject(hbwindow);
  DeleteDC(hwindowCompatibleDC);
  DeleteDC(hwindowDC);

  return src;
}

cv::Rect screen_position(char const *window_name) {
  HDC hwindowDC = GetDC(nullptr);
  int width = GetDeviceCaps(hwindowDC, HORZRES);
  int height = GetDeviceCaps(hwindowDC, VERTRES);
  DeleteDC(hwindowDC);

  auto window = FindWindow(nullptr, window_name);
  RECT windowsize;
  GetWindowRect(window, &windowsize);

  return cv::Rect(
      std::clamp<long>(windowsize.left, 0, width),
      std::clamp<long>(windowsize.top, 0, height),
      std::clamp<long>(windowsize.right - windowsize.left, 0, width),
      std::clamp<long>(windowsize.bottom - windowsize.top, 0, height));
}

static bool close_pixel(const cv::Vec3b &lhs, const cv::Vec3b &rhs) {
  static constexpr size_t close_ness = 3;
  return std::abs(lhs[0] - rhs[0]) < close_ness &&
         std::abs(lhs[1] - rhs[1]) < close_ness &&
         std::abs(lhs[2] - rhs[2]) < close_ness;
}

std::vector<mi::Piece> read_board(cv::Mat const &board_image) {
  // With a resolution of 1200x800
  // Every square is 88x88 pixels, the board is 8x8 squares.
  // Start of board is 91r, 296c

  static constexpr size_t row_start = 91;
  static constexpr size_t col_start = 296;
  static constexpr size_t square_height = 88;
  static constexpr size_t square_width = 88;
  static constexpr size_t board_rows = 8;
  static constexpr size_t board_cols = 8;

  // For coins
  static constexpr size_t row_offset_1 = 40;
  static constexpr size_t col_offset_1 = 40;

  // For chests
  static constexpr size_t row_offset_2 = 63;
  static constexpr size_t col_offset_2 = 45;

  // For vault
  static constexpr size_t row_offset_3 = 28;
  static constexpr size_t col_offset_3 = 35;

  // For sack
  static constexpr size_t row_offset_4 = 66;
  static constexpr size_t col_offset_4 = 25;

  static const cv::Vec3b copper_color_1{38, 42, 132};
  static const cv::Vec3b copper_color_2{77, 106, 178};
  static const cv::Vec3b copper_color_3{49, 81, 173};

  static const cv::Vec3b silver_color_1{112, 98, 74};
  static const cv::Vec3b silver_color_2{210, 203, 171};
  static const cv::Vec3b silver_color_3{158, 145, 110};

  static const cv::Vec3b gold_color_1{35, 110, 192};
  static const cv::Vec3b gold_color_2{82, 195, 232};
  static const cv::Vec3b gold_color_3{49, 173, 222};

  static const cv::Vec3b sack_color_1{57, 85, 140};
  static const cv::Vec3b sack_color_2{39, 50, 103};
  static const cv::Vec3b sack_color_3{41, 49, 95};
  static const cv::Vec3b sack_color_4{66, 44, 42};
  static const cv::Vec3b sack_color_5{72, 51, 56};

  static const cv::Vec3b brown_chest_color_1{216, 219, 223};
  static const cv::Vec3b brown_chest_color_2{16, 75, 171};
  static const cv::Vec3b brown_chest_color_3{247, 247, 247};

  static const cv::Vec3b green_chest_color_1{95, 170, 197};
  static const cv::Vec3b green_chest_color_2{56, 167, 23};
  static const cv::Vec3b green_chest_color_3{220, 239, 243};

  static const cv::Vec3b red_chest_color_1{24, 48, 75};
  static const cv::Vec3b red_chest_color_2{40, 0, 163};
  static const cv::Vec3b red_chest_color_3{202, 223, 227};

  static const cv::Vec3b vault_color_1{2, 8, 13};
  static const cv::Vec3b vault_color_2{25, 43, 54};
  static const cv::Vec3b vault_color_3{23, 51, 65};

  std::vector<match_idle::Piece> board{board_rows * board_cols};
  using T = mi::Piece::Type;

  for (size_t r = 0; r < 8; r++) {
    for (size_t c = 0; c < 8; c++) {
      const int row = row_start + r * square_height;
      const int col = col_start + c * square_width;
      const auto pixel_1 =
          board_image.at<cv::Vec3b>(row + row_offset_1, col + col_offset_1);
      const auto pixel_2 =
          board_image.at<cv::Vec3b>(row + row_offset_2, col + col_offset_2);
      const auto pixel_3 =
          board_image.at<cv::Vec3b>(row + row_offset_3, col + col_offset_3);
      const auto pixel_4 =
          board_image.at<cv::Vec3b>(row + row_offset_4, col + col_offset_4);

      T type{T::Empty};

      if (close_pixel(pixel_1, copper_color_1) ||
          close_pixel(pixel_2, copper_color_2) ||
          close_pixel(pixel_3, copper_color_3)) {
        type = T::Copper;
      } else if (close_pixel(pixel_1, silver_color_1) ||
                 close_pixel(pixel_2, silver_color_2) ||
                 close_pixel(pixel_3, silver_color_3)) {
        type = T::Silver;
      } else if (close_pixel(pixel_1, gold_color_1) ||
                 close_pixel(pixel_2, gold_color_2) ||
                 close_pixel(pixel_3, gold_color_3)) {
        type = T::Gold;
      } else if (close_pixel(pixel_1, sack_color_1) ||
                 close_pixel(pixel_2, sack_color_2) ||
                 close_pixel(pixel_3, sack_color_3) ||
                 close_pixel(pixel_4, sack_color_4) ||
                 close_pixel(pixel_4, sack_color_5)) {
        type = T::Sack;
      } else if (close_pixel(pixel_1, brown_chest_color_1) ||
                 close_pixel(pixel_2, brown_chest_color_2) ||
                 close_pixel(pixel_3, brown_chest_color_3)) {
        type = T::BrownChest;
      } else if (close_pixel(pixel_1, green_chest_color_1) ||
                 close_pixel(pixel_2, green_chest_color_2) ||
                 close_pixel(pixel_3, green_chest_color_3)) {
        type = T::GreenChest;
      } else if (close_pixel(pixel_1, red_chest_color_1) ||
                 close_pixel(pixel_2, red_chest_color_2) ||
                 close_pixel(pixel_3, red_chest_color_3)) {
        type = T::RedChest;
      } else if (close_pixel(pixel_1, vault_color_1) ||
                 close_pixel(pixel_2, vault_color_2) ||
                 close_pixel(pixel_3, vault_color_3)) {
        type = T::Vault;
      } else {
        fmt::print(
            "Unrecognized pixel: r {} c {} b1 {} g1 {} r1 {} b2 {} g2 {} r2 {} "
            "b3 {} g3 {} r3 {}\n",
            r, c, pixel_1[0], pixel_1[1], pixel_1[2], pixel_2[0], pixel_2[1],
            pixel_2[2], pixel_4[0], pixel_4[1], pixel_4[2]);
        // throw std::runtime_error("Unrecognized pixel");
      }

      board[r * board_rows + c] = mi::Piece(type);
    }
  }

  return board;
}

}  // namespace cheat
