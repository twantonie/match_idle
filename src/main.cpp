#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <fmt/core.h>

#include <centurion.hpp>

#include "match_window.hpp"

using namespace match_idle;

enum class State { MainMenu, Game };

struct GameData {
  cen::window window{"Centurion window", cen::window::default_size(),
                     cen::window::default_flags()};
  cen::renderer renderer{window};

  std::mt19937 gen{std::random_device{}()};
  MatchArea match_area{{100, 100, 400, 400}, gen};

  State state{State::MainMenu};

  bool running{true};
};

void handle_main_menu(GameData *data) {
  cen::event event;
  while (event.poll()) {
    // Check if the user wants to quit the application
    if (event.is<cen::quit_event>()) {
      data->running = false;
      break;
    }

    // Check for key press
    if (event.is<cen::keyboard_event>() ||
        event.is<cen::mouse_button_event>()) {
      data->match_area =
          MatchArea{{100, 100, 400, 400}, std::mt19937{std::random_device{}()}};
      data->state = State::Game;
    }
  }

  data->renderer.clear_with(cen::colors::black);

  cen::font font{"C:/Code/match_idle/resources/daniel.ttf", 36};

  data->renderer.set_color(cen::colors::white);
  auto title = data->renderer.render_solid_utf8("Main menu", font);

  data->renderer.render(title, cen::ipoint{0, 0});

  auto press = data->renderer.render_solid_utf8("press any key to start", font);
  data->renderer.render(press, cen::ipoint{0, title.size().height + 10});

  data->renderer.present();
}

void handle_match(GameData *data) {
  cen::event event;
  while (event.poll()) {
    // Check if the user wants to quit the application
    if (event.is<cen::quit_event>()) {
      data->running = false;
      break;
    }

    data->match_area.handle_events(event);
  }

  if (data->match_area.game_over()) {
    data->state = State::MainMenu;
  }

  data->renderer.clear_with(cen::colors::white);

  data->match_area.render(data->renderer);

  data->renderer.present();
}

void loop(void *in_data) {
  auto data = static_cast<GameData *>(in_data);

  switch (data->state) {
    case State::MainMenu:
      handle_main_menu(data);
      break;
    case State::Game:
      handle_match(data);
      break;
  }
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