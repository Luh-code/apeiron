#include "apeiron_core/apeiron_core.hpp"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <cstdint>
#include <limits.h>
#include <loguru.hpp>
#include <valarray>

void print_error(std::string msg, int32_t ret_code, std::string desc) {
  printf((std::string("%s (code: %d)") + (desc != "" ? "  --> %s\n" : "%s"))
             .c_str(),
         msg.c_str(), ret_code, desc.c_str());
}

int32_t init_sdl_window(SDL_Window *&window) {
  if (auto ret = apeiron_core::window::sdl_init(static_cast<int32_t>(0));
      ret != apeiron_core::Errors::SUCCESS) {
    print_error("Failed to initialize the SDL2 libaray!", ret,
                apeiron_core::window::sdl_get_error());
    return ret;
  }

  {
    apeiron_core::window::WindowCreateInfo create_info{
        .str_title = "SDL Window",
        ._posx = SDL_WINDOWPOS_CENTERED,
        ._posy = SDL_WINDOWPOS_CENTERED,
        ._sizex = 800,
        ._sizey = 600,
    };
    if (auto ret = apeiron_core::window::sdl_create_window(window, create_info);
        ret != apeiron_core::Errors::SUCCESS) {
      print_error("Failed to create window!", ret,
                  apeiron_core::window::sdl_get_error());
      return ret;
    }
  }

  SDL_Surface *surface = nullptr;
  if (auto ret = apeiron_core::window::sdl_get_surface(surface, window);
      ret != apeiron_core::Errors::SUCCESS) {
    print_error("Failed to get the surface from the window!", ret,
                apeiron_core::window::sdl_get_error());
    return ret;
  }

  SDL_UpdateWindowSurface(window);

  return 0;
}

int32_t main_loop(int32_t test, apeiron_core::ApplicationData &app_data) {
  LOG_SCOPE_F(INFO, "Main Loop");
  bool running = true;
  while (running == true) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        running = false;
        LOG_F(INFO, "Quitting application");
        break;
      default:
        break;
      }
    }
  }
  return test;
}

int main(int32_t argc, char *argv[]) {
  loguru::init(argc, argv);
  char log_path[PATH_MAX];
  loguru::suggest_log_path("./logs/", log_path, sizeof(log_path));
  loguru::add_file(log_path, loguru::FileMode::Truncate, loguru::Verbosity_MAX);

  apeiron_core::App<apeiron_core::ApplicationCreateInfo *,
                    apeiron_core::ApplicationData &, int32_t,
                    apeiron_core::ApplicationData &, int32_t,
                    apeiron_core::ApplicationData &>
      app{
          .p_init = apeiron_core::normal_init,
          .p_main_loop = main_loop,
          .p_cleanup = apeiron_core::normal_cleanup,
      };

  apeiron_core::window::WindowCreateInfo window_create_info{
      ._windowType = apeiron_core::window::WindowType::SDL,
      // ._windowType = 4,
      .str_title = "SDL Vulkan window",
      ._posx = SDL_WINDOWPOS_CENTERED,
      ._posy = SDL_WINDOWPOS_CENTERED,
      ._sizex = 800,
      ._sizey = 600,
  };
  apeiron_core::ApplicationCreateInfo create_info{
      .p_windowCreateInfo = &window_create_info,
  };
  apeiron_core::ApplicationData app_data{

  };
  int32_t m = 0, c = 0;
  apeiron_core::AppBootstrap<apeiron_core::ApplicationCreateInfo *,
                             apeiron_core::ApplicationData &, int32_t,
                             apeiron_core::ApplicationData &, int32_t,
                             apeiron_core::ApplicationData &>
      app_bootstrap{
          .p_app = &app,

          ._i = &create_info,
          ._ir = app_data,
          ._m = m,
          ._mr = app_data,
          ._c = c,
          ._cr = app_data,
      };
  apeiron_core::ApplicationCreateInfo *p = &create_info;
  if (auto ret = apeiron_core::run_app(app_bootstrap);
      ret != apeiron_core::Errors::SUCCESS) {
    LOG_F(INFO, "An error occured whilst running app! (code: %d)", ret);
    return -1;
  }

  return 0;
}
