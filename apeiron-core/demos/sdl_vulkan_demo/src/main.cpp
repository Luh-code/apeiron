#include "apeiron_core/apeiron_core.hpp"

void print_error(std::string msg, int32_t ret_code, std::string desc) {
  printf((std::string("%s (code: %d)") + (desc != "" ? "  --> %s\n" : "%s"))
             .c_str(),
         msg.c_str(), ret_code, desc.c_str());
}

int32_t init_sdl_window(SDL_Window *&window) {
  if (auto ret = apeiron_core::window::sdl_init(static_cast<int32_t>(0));
      ret != apeiron_core::errors::SUCCESS) {
    print_error("Failed to initialize the SDL2 libaray!", ret,
                apeiron_core::window::sdl_get_error());
    return ret;
  }

  {
    apeiron_core::window::WindowCreateInfo create_info{
        .title = "SDL Window",
        .posx = SDL_WINDOWPOS_CENTERED,
        .posy = SDL_WINDOWPOS_CENTERED,
        .sizex = 800,
        .sizey = 600,
    };
    if (auto ret = apeiron_core::window::sdl_create_window(window, create_info);
        ret != apeiron_core::errors::SUCCESS) {
      print_error("Failed to create window!", ret,
                  apeiron_core::window::sdl_get_error());
      return ret;
    }
  }

  SDL_Surface *surface = nullptr;
  if (auto ret = apeiron_core::window::sdl_get_surface(surface, window);
      ret != apeiron_core::errors::SUCCESS) {
    print_error("Failed to get the surface from the window!", ret,
                apeiron_core::window::sdl_get_error());
    return ret;
  }

  SDL_UpdateWindowSurface(window);

  return 0;
}

int32_t main_loop(int32_t test) {
  std::cout << "Main loop reached!!!\n";
  return test;
}

int main() {
  // SDL_Window *window;
  // if (auto ret = init_sdl_window(window); ret != 0) {
  //   return -1;
  // }

  apeiron_core::App<int32_t, int32_t, int32_t> app{
      .init = apeiron_core::normal_init,
      .main_loop = main_loop,
      .cleanup = apeiron_core::normal_cleanup,
  };

  int32_t i = 0, m = -3, c = 0;
  if (auto ret = apeiron_core::run_app(app, i, m, c);
      ret != apeiron_core::errors::SUCCESS) {
    print_error("An error occured whilst running app!", ret, "");
    return -1;
  }

  return 0;
}
