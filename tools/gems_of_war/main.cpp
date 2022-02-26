#include <fmt/core.h>
#include <windows.h>

int main() {
  auto window = FindWindow(nullptr, "GemsOfWar");
  if (!window) {
    fmt::print("Couldn't find window\n");
    return 1;
  }

  fmt::print("Found window\n");

  SetForegroundWindow(window);
  Sleep(10);

  HDC hScreenDC = GetDC(window);
  HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
  int width = GetDeviceCaps(hScreenDC, HORZRES);
  int height = GetDeviceCaps(hScreenDC, VERTRES);
  HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
  HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(hMemoryDC, hBitmap));
  BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);
  hBitmap = static_cast<HBITMAP>(SelectObject(hMemoryDC, hOldBitmap));

  // save bitmap to clipboard
  OpenClipboard(NULL);
  EmptyClipboard();
  SetClipboardData(CF_BITMAP, hBitmap);
  CloseClipboard();

  DeleteDC(hMemoryDC);
  DeleteDC(hScreenDC);
}