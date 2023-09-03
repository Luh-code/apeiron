#include "app.hpp"

namespace apeiron_core {
int32_t init_window(window::WindowCreateInfo *create_info) {
  LOG_SCOPE_F(INFO, "Initializing window");
  // Create window according to _windowType of create_info
  switch (create_info->_windowType) {
  case window::WindowType::SDL:
    LOG_F(INFO, "Type: SDL window");
    if (!window::sdl_initialized(0)) {
      LOG_F(INFO, "Automatically initialized SDL");
      window::sdl_init(0);
    }
    return init_SDL_window(create_info);
  case window::WindowType::GLFW:
    LOG_F(INFO, "Type: GLFW window");
    LOG_F(ERROR, "GLFW windows are not jet implemented!");
    // init_GLFW_window(create_info);
    return Errors::NOT_IMPLEMENTED_ERROR;
  default:
    LOG_F(ERROR, "'%d' is not a legal window type!", create_info->_windowType);
    return Errors::WINDOW_WINDOWTYPE_NOT_LEGAL;
  }
  return Errors::SUCCESS;
}

int32_t init_SDL_window(window::WindowCreateInfo *create_info) {
  if (auto ret = window::sdl_initialized(0); !ret) {
    LOG_F(ERROR, "SDL is not initialized!");
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
  if (!create_info->p_windowCreateInfo) {
    LOG_F(ERROR, "p_windowCreateInfo is not defined!");
    return Errors::WINDOW_CREATE_INFO_NOT_DEFINED;
  }
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
