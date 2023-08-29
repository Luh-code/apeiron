#include "sdl_main.hpp"

namespace apeiron_core::window {
int sdl_init(int32_t flags) {
  flags |= SDL_INIT_VIDEO;
  if (SDL_WasInit(flags) != 0) {
    return ALREADY_INITIALIZED;
  }

  int32_t val = SDL_Init(flags);

  if (val != 0) {
    return INITIALIZATION_FAILED;
  }

  return SUCCESS;
}

int32_t sdl_create_window(SDL_Window *&window, WindowCreateInfo &create_info) {
  window =
      SDL_CreateWindow(create_info.title, create_info.posx, create_info.posy,
                       create_info.sizex, create_info.sizey, 0);

  if (!window) {
    return FAILED_WINDOW_CREATION;
  }

  return SUCCESS;
}

int32_t sdl_get_surface(SDL_Surface *&surface, SDL_Window *window) {
  surface = SDL_GetWindowSurface(window);

  if (!surface) {
    return FAILED_TO_GET_SURFACE;
  }

  return SUCCESS;
}

int32_t sdl_deinit(uint32_t flags) {
  flags |= SDL_INIT_VIDEO;
  if (SDL_WasInit(flags) == 0) {
    return NOT_INITIALIZED;
  }

  SDL_Quit();
  return SUCCESS;
}

const char *sdl_get_error() { return SDL_GetError(); }

int32_t sdl_test(uint32_t millis) {
  if (auto ret = sdl_init(static_cast<int32_t>(0)); ret != SUCCESS) {
    std::cout << "Failed to initialize the SDL2 library\n";
    return ret;
  }

  SDL_Window *window = nullptr;
  {
    WindowCreateInfo create_info{
        .title = "SDL Window",
        .posx = SDL_WINDOWPOS_CENTERED,
        .posy = SDL_WINDOWPOS_CENTERED,
        .sizex = 800,
        .sizey = 600,
    };
    if (auto ret = sdl_create_window(window, create_info); ret != SUCCESS) {
      std::cout << "Failed to create window\n";
      return ret;
    }
  }

  SDL_Surface *surface = nullptr;
  if (auto ret = sdl_get_surface(surface, window); ret != SUCCESS) {
    std::cout << "Failed to get the surface from the window \"" << window
              << "\" (code: " << ret << ")\n"
              << "  --> " << sdl_get_error() << std::endl;
    return ret;
  }

  SDL_UpdateWindowSurface(window);

  SDL_Delay(millis);

  return 0;
}
} // namespace apeiron_core::window
