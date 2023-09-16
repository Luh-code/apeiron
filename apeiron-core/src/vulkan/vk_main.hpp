#ifndef __VK_MAIN_HPP__
#define __VK_MAIN_HPP__

#include "../app/app_data.hpp"
#include "../window/sdl/sdl_main.hpp"
#include "../window/window.hpp"

#include <cstdint>
#include <string>

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan.h>

#include <vector>
#include <vulkan/vulkan_core.h>

namespace apeiron_core::vk {

struct InstanceCreateInfo {
public:
  const char *str_applicationName;
  uint32_t _version;

  std::vector<const char *> v_extensions;
  bool b_queryForExtensions;

  std::vector<const char *> v_layers;
  bool b_enableValidationLayers =
#ifdef NDEBUG
      false
#else
      true
#endif // NDEBUG
      ;
  std::vector<const char *> v_validationLayers = {
      "VK_LAYER_KHRONOS_validation",
  };
};
[[nodiscard]] int32_t create_instance(ApplicationData &app_data,
                                      InstanceCreateInfo &create_info);
static inline VKAPI_ATTR VkBool32 VKAPI_CALL
debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
               VkDebugUtilsMessageTypeFlagsEXT message_type,
               const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
               void *user_data) {
  const char *type;
  switch (message_type) {
  case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
    type = "general";
    break;
  case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
    type = "validation";
    break;
  case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
    type = "performance";
    break;
  default:
    LOG_F(ERROR, "Unknown Vulkan message type '%d'", message_type);
    type = "unknown";
    break;
  }
  const char *format = "Validation layer (%s): %s";
  switch (message_severity) {
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    LOG_F(ERROR, format, type, callback_data->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    LOG_F(WARNING, format, type, callback_data->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    VLOG_F(3, format, type, callback_data->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    VLOG_F(4, format, type, callback_data->pMessage);
    break;
  default:
    LOG_F(ERROR,
          "Vulkan message severity '%d' not recognized, treating as error",
          message_severity);
    LOG_F(ERROR, format, type, callback_data->pMessage);
    break;
  }
  return VK_FALSE;
}
struct DebugMessengerCreateInfo {
  VkDebugUtilsMessageSeverityFlagsEXT _severity;
  VkDebugUtilsMessageTypeFlagsEXT _types;
  // VKAPI_ATTR VkBool32 VKAPI_CALL (*p_callback)(
  //     VkDebugUtilsMessageSeverityFlagsEXT,
  //     VkDebugUtilsMessageTypeFlagBitsEXT, const
  //     VkDebugUtilsMessengerCallbackDataEXT *, void *);
  PFN_vkDebugUtilsMessengerCallbackEXT p_callback;
};
[[nodiscard]] int32_t
setup_debug_messenger(ApplicationData &app_data,
                      DebugMessengerCreateInfo &create_info);
} // namespace apeiron_core::vk

#endif // __VK_MAIN_HPP__
