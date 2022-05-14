#include "window_recognition.hpp"

#include <Windows.h>
#include <fmt/core.h>

#include <stdexcept>

namespace cheat {

cv::Mat take_screenshot() {
  // Copied from
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

cv::Mat resize_to_screen(const cv::Mat &screenshot, const char *window_name) {
  auto window = FindWindow(nullptr, window_name);
  if (!window) {
    throw std::runtime_error("Couldn't find window");
  }

  // get the height and width of the screen
  RECT windowsize;
  GetWindowRect(window, &windowsize);

  return screenshot(
      cv::Range(std::clamp<long>(windowsize.top, 0, screenshot.rows),
                std::clamp<long>(windowsize.bottom, 0, screenshot.rows)),
      cv::Range(std::clamp<long>(windowsize.left, 0, screenshot.cols),
                std::clamp<long>(windowsize.right, 0, screenshot.cols)));
}

}  // namespace cheat
