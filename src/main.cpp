#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <centurion.hpp>

#include "match_window.hpp"

using namespace match_idle;

struct GameData {
  cen::window window{"Centurion window", cen::window::default_size(),
                     cen::window::default_flags()};
  cen::renderer renderer{window};
  cen::event event;

  std::mt19937 gen{std::random_device{}()};
  MatchArea match_area{{100, 100, 400, 400}, gen};

  bool running{true};
};

void loop(void *in_data) {
  auto data = static_cast<GameData *>(in_data);

  while (data->event.poll()) {
    // Check if the user wants to quit the application
    if (data->event.is<cen::quit_event>()) {
      data->running = false;
      break;
    }

    data->match_area.handle_events(data->event);
  }

  if (data->match_area.game_over()) {
    data->running = false;
  }

  data->renderer.clear_with(cen::colors::white);

  data->match_area.render(data->renderer);

  data->renderer.present();
}

int main(int, char **) {
  // Initialize the library
  cen::library centurion;

  // Create a window and an associated renderer
  GameData data;

  // Make sure our window is visible
  data.window.set_title("Match Idle");
  data.window.show();

  // Doesn't look like I can resize textures/surfaces
  // constexpr char sheep_loc[] = "C:/Code/match_idle/resources/sheep.svg";
  // cen::texture sheep(renderer, sheep_loc);
  //   renderer.render(sheep, cen::ipoint(0, 0));

  constexpr uint32_t fps = 60;

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(loop, &data, fps, 1);
#else

  auto start_time = cen::counter::ticks();

  constexpr std::chrono::milliseconds time_per_frame{1000 / fps};
  while (data.running) {
    loop(&data);

    const auto curr_time = cen::counter::ticks();
    const auto delta = curr_time - start_time;
    if (delta < time_per_frame) {
      cen::thread::sleep(time_per_frame - delta);
    }
    start_time = curr_time;
  }

  // Make the window invisible again, this might not be necessary, but it
  // doesn't hurt
  data.window.hide();
#endif

  return 0;
}