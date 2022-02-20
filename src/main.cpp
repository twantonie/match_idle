#include <centurion.hpp>

#include "match_window.hpp"

using namespace match_idle;

int main(int, char**) {
  // Initialize the library
  cen::library centurion;

  // Create a window and an associated renderer
  auto [window, renderer] = cen::make_window_and_renderer();

  // Make sure our window is visible
  window.set_title("Match Idle");
  window.show();

  cen::event event;
  bool running = true;

  std::random_device dev;
  std::mt19937 gen(dev());
  MatchArea match_area({100, 100, 400, 400}, gen);

  // Doesn't look like I can resize textures/surfaces
  // constexpr char sheep_loc[] = "C:/Code/match_idle/resources/sheep.svg";
  // cen::texture sheep(renderer, sheep_loc);
  //   renderer.render(sheep, cen::ipoint(0, 0));

  auto start_time = cen::counter::ticks();

  constexpr uint32_t fps = 60;
  constexpr std::chrono::milliseconds time_per_frame{1000 / fps};

  while (running) {
    while (event.poll()) {
      // Check if the user wants to quit the application
      if (event.is<cen::quit_event>()) {
        running = false;
        break;
      }

      match_area.handle_events(event);
    }

    if (match_area.game_over()) return -1;

    renderer.clear_with(cen::colors::white);

    match_area.render(renderer);

    renderer.present();

    const auto curr_time = cen::counter::ticks();
    const auto delta = curr_time - start_time;
    if (delta < time_per_frame) {
      cen::thread::sleep(time_per_frame - delta);
    }
    start_time = curr_time;
  }

  // Make the window invisible again, this might not be necessary, but it
  // doesn't hurt
  window.hide();

  return 0;
}