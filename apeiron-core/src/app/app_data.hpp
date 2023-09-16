#ifndef __APP_DATA_HPP__
#define __APP_DATA_HPP__

#include <SDL2/SDL_video.h>
#include <cstdint>
#include <vulkan/vulkan.h>

namespace apeiron_core {

struct ApplicationData {
public:
  int32_t _windowType;
  SDL_Window *p_SDLWindow;
  SDL_Surface *p_SDLSurface;

  VkAllocationCallbacks *p_allocator;
  VkInstance _instance;
  VkDebugUtilsMessengerEXT _debugMessenger;
};
} // namespace apeiron_core

#endif // !__APP_DATA_HPP__
