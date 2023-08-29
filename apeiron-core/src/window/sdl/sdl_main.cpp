#include "sdl_main.hpp"

namespace apeiron_core::window {
void sdl_test() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "Failed to initialize the SDL2 library\n";
    return;
  }

  SDL_Window *window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, 680, 480, 0);

  if (!window) {
    std::cout << "Failed to create window\n";
    return;
  }

  SDL_Surface *window_surface = SDL_GetWindowSurface(window);

  if (!window_surface) {
    std::cout << "Failed to get the surface from the window\n";
    return;
  }

  SDL_UpdateWindowSurface(window);

  SDL_Delay(5000);
}
} // namespace apeiron_core::window
