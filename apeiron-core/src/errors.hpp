#ifndef __ERRORS_HPP__
#define __ERRORS_HPP__

#include <cstdint>

namespace apeiron_core {
/*
 * >0 = Warning
 * 0 = Positive
 * <0 = Error
 */
enum Errors : int32_t {
  // Vulkan Warnings (9000, 9999)
  VULKAN_WARNING = 9000,
  // GLFW Warnings (8000, 8999)
  GLFW_WARNING = 8000,
  // SDL Warnings (7000, 7999)
  SDL_WARNING = 7000,
  // Window Warnings (5000, 6999)
  WINDOW_WARNING = 5000,
  // Generic Warnings (1, 4999)
  GENERIC_WARNING = 1,
  // Positive (0)
  SUCCESS = 0,
  // Generic Errors (-1, -4999)
  ERROR = -1,
  NOT_IMPLEMENTED_ERROR = -2,
  // Window Errors (-5000, -6999)
  WINDOW_ERROR = -5000,
  WINDOW_CREATE_INFO_NOT_DEFINED = -5001,
  WINDOW_WINDOWTYPE_NOT_LEGAL = -5002,
  // SDL Errors (-7000, -7999)
  SDL_ERROR = -7000,
  SDL_FAILED_WINDOW_CREATION = -7001,
  SDL_FAILED_TO_GET_SURFACE = -7002,
  SDL_ALREADY_INITIALIZED = -7003,
  SDL_NOT_INITIALIZED = -7004,
  SDL_FAILED_INITIALIZATION = -7005,
  SDL_FAILED_TO_GET_VK_INSTANCE_EXTENSIONS = -7006,
  SDL_FAILED_TO_GET_VK_INSTANCE_EXTENSION_COUNT = -7007,
  // GLFW Errors (-8000, -8999)
  GLFW_ERROR = -8000,
  // Vulkan Errors (-9000, -9999)
  VULKAN_ERROR = -9000,
  VULKAN_FAILED_TO_CREATE_INSTANCE = -9001,
};
} // namespace apeiron_core

#endif // !__ERRORS_HPP__
