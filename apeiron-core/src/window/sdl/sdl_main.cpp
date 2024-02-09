#include "sdl_main.hpp"
#include <SDL2/SDL_video.h>

namespace apeiron_core::window {
int32_t sdl_init(int32_t flags) {
  flags |= SDL_INIT_VIDEO | SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN;
  if (SDL_WasInit(flags) != 0) {
    return apeiron_core::Errors::SDL_ALREADY_INITIALIZED;
  }

  int32_t val = SDL_Init(flags);

  if (val != 0) {
    return apeiron_core::Errors::SDL_FAILED_INITIALIZATION;
  }

  return apeiron_core::Errors::SUCCESS;
}

bool sdl_initialized(int32_t flags) {
  flags |= SDL_INIT_VIDEO | SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN;

  return SDL_WasInit(flags);
}

int32_t sdl_create_window(SDL_Window *&window, WindowCreateInfo &create_info) {
  VLOG_F(2, "Creating SDL_Window '%s' of size %dx%d", create_info.str_title,
         create_info._sizex, create_info._sizey);

  window = SDL_CreateWindow(
      create_info.str_title, create_info._posx, create_info._posy,
      create_info._sizex, create_info._sizey,
      SDL_INIT_VIDEO | SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);

  if (!window) {
    return apeiron_core::Errors::SDL_FAILED_WINDOW_CREATION;
  }

  VLOG_F(2, "Window: %li", reinterpret_cast<int64_t>(window));
  return apeiron_core::Errors::SUCCESS;
}

int32_t sdl_get_surface(SDL_Surface *&surface, SDL_Window *&window) {
  surface = SDL_GetWindowSurface(window);

  if (!surface) {
    return apeiron_core::Errors::SDL_FAILED_TO_GET_SURFACE;
  }

  return apeiron_core::Errors::SUCCESS;
}

int32_t sdl_destroy_window(SDL_Window *&window) {
  SDL_DestroyWindow(window);
  return Errors::SUCCESS;
}

int32_t sdl_deinit(uint32_t flags) {
  flags |= SDL_INIT_VIDEO;
  if (SDL_WasInit(flags) == 0) {
    return apeiron_core::Errors::SDL_NOT_INITIALIZED;
  }

  SDL_Quit();
  return apeiron_core::Errors::SUCCESS;
}

int32_t sdl_get_instance_extensions(SDL_Window *window,
                                    uint32_t *extension_count,
                                    const char ***extensions) {
  if (SDL_Vulkan_GetInstanceExtensions(window, extension_count, 0) ==
      SDL_FALSE) {
    return Errors::SDL_FAILED_TO_GET_VK_INSTANCE_EXTENSION_COUNT;
  }
  *extensions = new const char *[*extension_count];
  if (SDL_Vulkan_GetInstanceExtensions(window, extension_count, *extensions) ==
      SDL_FALSE) {
    return Errors::SDL_FAILED_TO_GET_VK_INSTANCE_EXTENSIONS;
  }
  return Errors::SUCCESS;
}

const char *sdl_get_error() { return SDL_GetError(); }

int32_t sdl_test(uint32_t millis) {
  if (auto ret = sdl_init(static_cast<int32_t>(0));
      ret != apeiron_core::Errors::SUCCESS) {
    std::cout << "Failed to initialize the SDL2 library\n";
    return ret;
  }

  SDL_Window *window = nullptr;
  {
    WindowCreateInfo create_info{
        .str_title = "SDL Window",
        ._posx = SDL_WINDOWPOS_CENTERED,
        ._posy = SDL_WINDOWPOS_CENTERED,
        ._sizex = 800,
        ._sizey = 600,
    };
    if (auto ret = sdl_create_window(window, create_info);
        ret != apeiron_core::Errors::SUCCESS) {
      std::cout << "Failed to create window\n";
      return ret;
    }
  }

  SDL_Surface *surface = nullptr;
  if (auto ret = sdl_get_surface(surface, window);
      ret != apeiron_core::Errors::SUCCESS) {
    std::cout << "Failed to get the surface from the window \"" << window
              << "\" (code: " << ret << ")\n"
              << "  --> " << sdl_get_error() << std::endl;
    return ret;
  }

  SDL_UpdateWindowSurface(window);

  SDL_Delay(millis);

  return 0;
}

int32_t sdl_create_vk_surface_khr(ApplicationData &app_data) {
  if (!sdl_initialized(SDL_WINDOW_VULKAN)) {
    LOG_F(ERROR, "SDL_window not initialized for use with Vulkan! "
                 "(SDL_WINDOW_VULKAN flag missing)");
    return Errors::SDL_NOT_INITIALIZED;
  }
  if (SDL_Vulkan_CreateSurface(app_data.p_SDLWindow, app_data._instance,
                               &app_data._surface)) {
    return Errors::SUCCESS;
  }
  return Errors::SDL_FAILED_TO_CREATE_VK_SURFACE_KHR;
}
} // namespace apeiron_core::window
