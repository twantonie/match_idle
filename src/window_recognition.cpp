#include "window_recognition.hpp"

#include <fmt/core.h>

#include <random>
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
  static constexpr size_t close_ness = 5;
  return std::abs(lhs[0] - rhs[0]) <= close_ness &&
         std::abs(lhs[1] - rhs[1]) <= close_ness &&
         std::abs(lhs[2] - rhs[2]) <= close_ness;
}

constexpr size_t row_start = 148;
constexpr size_t col_start = 273;

constexpr size_t square_height = 94;
constexpr size_t square_width = 94;

static constexpr size_t board_rows = 8;
static constexpr size_t board_cols = 8;

cv::Rect board_position() {
  return cv::Rect(col_start, row_start, square_width * board_cols,
                  square_height * board_rows);
}

std::vector<mi::Piece> read_board(cv::Mat const &board_image) {
  static constexpr size_t row_offset_coin = 40;
  static constexpr size_t col_offset_coin = 40;

  static constexpr size_t row_offset_chest = 60;
  static constexpr size_t col_offset_chest = 22;

  static constexpr size_t row_offset_vault = 28;
  static constexpr size_t col_offset_vault = 35;

  static constexpr size_t row_offset_sack = 66;
  static constexpr size_t col_offset_sack = 25;

  static constexpr size_t row_offset_green_chest = 65;
  static constexpr size_t col_offset_green_chest = 53;

  static const cv::Vec3b copper_color{38, 42, 132};
  static const cv::Vec3b silver_color{112, 98, 74};
  static const cv::Vec3b gold_color{35, 110, 192};

  static const cv::Vec3b sack_color_1{75, 53, 59};
  static const cv::Vec3b sack_color_2{81, 56, 64};

  static const cv::Vec3b brown_chest_color{0, 16, 54};
  static const cv::Vec3b green_chest_color{69, 181, 24};
  static const cv::Vec3b red_chest_color{13, 0, 69};

  static const cv::Vec3b vault_color{23, 51, 65};

  std::vector<match_idle::Piece> board{board_rows * board_cols};
  using T = mi::Piece::Type;

  for (size_t r = 0; r < 8; r++) {
    for (size_t c = 0; c < 8; c++) {
      const int row = row_start + r * square_height;
      const int col = col_start + c * square_width;
      const auto coin_pixel = board_image.at<cv::Vec3b>(row + row_offset_coin,
                                                        col + col_offset_coin);
      const auto chest_pixel = board_image.at<cv::Vec3b>(
          row + row_offset_chest, col + col_offset_chest);
      const auto sack_pixel = board_image.at<cv::Vec3b>(row + row_offset_sack,
                                                        col + col_offset_sack);
      const auto green_chest_pixel = board_image.at<cv::Vec3b>(
          row + row_offset_green_chest, col + col_offset_green_chest);

      T type{T::Empty};

      if (close_pixel(coin_pixel, copper_color)) {
        type = T::Copper;
      } else if (close_pixel(coin_pixel, silver_color)) {
        type = T::Silver;
      } else if (close_pixel(coin_pixel, gold_color)) {
        type = T::Gold;
      } else if (close_pixel(sack_pixel, sack_color_1) ||
                 close_pixel(sack_pixel, sack_color_2)) {
        type = T::Sack;
      } else if (close_pixel(chest_pixel, brown_chest_color)) {
        type = T::BrownChest;
      } else if (close_pixel(green_chest_pixel, green_chest_color)) {
        type = T::GreenChest;
      } else if (close_pixel(chest_pixel, red_chest_color)) {
        type = T::RedChest;
      } else if (close_pixel(board_image.at<cv::Vec3b>(row + row_offset_vault,
                                                       col + col_offset_vault),
                             vault_color)) {
        type = T::Vault;
      } else {
        fmt::print(
            "Unrecognized pixel: r {} c {} coin {}:{}:{} chest {}:{}:{} sack "
            "{}:{}:{} green chest {}:{}:{}\n",
            r, c, coin_pixel[0], coin_pixel[1], coin_pixel[2], chest_pixel[0],
            chest_pixel[1], chest_pixel[2], sack_pixel[0], sack_pixel[1],
            sack_pixel[2], green_chest_pixel[0], green_chest_pixel[1],
            green_chest_pixel[2]);
        // throw std::runtime_error("Unrecognized pixel");
      }

      board[r * board_rows + c] = mi::Piece(type);
    }
  }

  return board;
}

std::pair<long, long> calculate_coordinates(mi::Point pos,
                                            const cv::Rect screen_pos,
                                            float x_offset, float y_offset) {
  std::pair<long, long> pair;
  auto &dx = pair.first;
  auto &dy = pair.second;

  dx = screen_pos.x + col_start + pos.col * square_width +
       x_offset * square_width;
  dy = screen_pos.y + row_start + pos.row * square_height +
       y_offset * square_height;

  return pair;
}

struct RandomHelper {
  std::mt19937 gen{std::random_device{}()};
  std::uniform_real_distribution<float> offset_dist{0.1, 0.9};
  std::bernoulli_distribution move_dist{0.5};

  float offset() { return offset_dist(gen); }

  bool move() { return move_dist(gen); }
};

std::pair<long, long> get_mouse_pos() {
  POINT point;
  GetCursorPos(&point);
  return std::make_pair(point.x, point.y);
}

void move_mouse(long x_pos, long y_pos) {
  INPUT Inputs[1]{};

  Inputs[0].type = INPUT_MOUSE;
  Inputs[0].mi.dx = x_pos * 25.6;
  Inputs[0].mi.dy = y_pos * 45.6;
  Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

  SendInput(1, Inputs, sizeof(INPUT));
}

void click_mouse() {
  INPUT Inputs[2]{};

  Inputs[0].type = INPUT_MOUSE;
  Inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

  Inputs[1].type = INPUT_MOUSE;
  Inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

  SendInput(2, Inputs, sizeof(INPUT));
}

void move_piece(match_idle::MoveDir move_dir, match_idle::Point pos,
                const cv::Rect screen_pos) {
  static RandomHelper random{};

  float x_start_offset = random.offset();
  float y_start_offset = random.offset();
  float x_end_offset = random.offset();
  float y_end_offset = random.offset();

  bool flip_move = random.move();

  auto new_pos = pos;
  if (move_dir == mi::MoveDir::Right) {
    new_pos.col += 1;
  } else {
    // Move dir down
    new_pos.row += 1;
  }

  if (flip_move) {
    std::swap(pos, new_pos);
  }

  // x == width; y == height
  auto [dx_start, dy_start] =
      calculate_coordinates(pos, screen_pos, x_start_offset, y_start_offset);
  auto [dx_end, dy_end] =
      calculate_coordinates(new_pos, screen_pos, x_end_offset, y_end_offset);

  move_mouse(dx_start, dy_start);
  click_mouse();

  Sleep(50);

  move_mouse(dx_end, dy_end);
  click_mouse();

  Sleep(50);

  move_mouse(screen_pos.x + col_start / 2, screen_pos.y + row_start / 2);
}

void register_key(Key key) {
  if (!RegisterHotKey(NULL, key.id, MOD_CONTROL | MOD_NOREPEAT, key.key)) {
    throw std::runtime_error(
        fmt::format("Failed to register key {}:{}", key.id, key.key));
  }
}

int get_hotkey_id() {
  MSG msg{};
  if (GetMessage(&msg, NULL, 0, 0) != 0) {
    if (msg.message == WM_HOTKEY) {
      return msg.wParam;
    }
  }

  return 0;
}

}  // namespace cheat
