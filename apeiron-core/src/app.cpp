#include "app.hpp"
#include "errors.hpp"
#include "window/sdl/sdl_main.hpp"
#include "window/window.hpp"

namespace apeiron_core {
int32_t init_window(window::WindowCreateInfo *create_info) {
  switch (create_info->_windowType) {
  case window::WindowType::SDL:
    if (!window::sdl_initialized(0)) {
      window::sdl_init(0);
    }
    return init_SDL_window(create_info);
  case window::WindowType::GLFW:
    // init_GLFW_window(create_info);
    break;
  }
  return Errors::SUCCESS;
}

int32_t init_SDL_window(window::WindowCreateInfo *create_info) {
  if (auto ret = window::sdl_initialized(0); !ret) {
    return Errors::SDL_NOT_INITIALIZED;
  }

  SDL_Window *window = nullptr;
  if (auto ret = window::sdl_create_window(window, *create_info);
      ret != Errors::SUCCESS) {
    return ret;
  }

  SDL_Surface *surface = nullptr;
  if (auto ret = window::sdl_get_surface(surface, window);
      ret != Errors::SUCCESS) {
    return ret;
  }

  SDL_UpdateWindowSurface(window);

  SDL_Delay(5000);

  return Errors::SUCCESS;
}

int32_t init_vulkan() { return Errors::SUCCESS; }

int32_t normal_init(ApplicationCreateInfo *create_info) {
  if (auto ret = init_window(create_info->p_windowCreateInfo);
      ret != Errors::SUCCESS) {
    return ret;
  }
  if (auto ret = init_vulkan(); ret != Errors::SUCCESS) {
    return ret;
  }
  return Errors::SUCCESS;
}

int32_t normal_cleanup(int32_t) { return Errors::SUCCESS; }
} // namespace apeiron_core
