#ifndef __VK_MAIN_HPP__
#define __VK_MAIN_HPP__

#include "../app/app_data.hpp"
#include "../window/sdl/sdl_main.hpp"
#include "../window/window.hpp"

#include <cstdint>
#include <map>
#include <optional>
#include <string>

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan.h>

#include <vector>
#include <vulkan/vulkan_core.h>

namespace apeiron_core::vk {

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
  PFN_vkDebugUtilsMessengerCallbackEXT p_callback;
};
void populate_debug_vk_messenger_create_info(
    VkDebugUtilsMessengerCreateInfoEXT &out, DebugMessengerCreateInfo &in);
VkResult create_debug_utils_messenger_ext(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *create_info,
    const VkAllocationCallbacks *allocator,
    VkDebugUtilsMessengerEXT *debug_messenger);
void destroy_debug_utils_messenger_ext(VkInstance instance,
                                       VkDebugUtilsMessengerEXT debug_messenger,
                                       const VkAllocationCallbacks *allocator);
[[nodiscard]] ap_error
setup_debug_messenger(ApplicationData &app_data,
                      DebugMessengerCreateInfo &create_info);

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

  DebugMessengerCreateInfo *p_debugMessengerCreateInfo;
};

[[nodiscard]] ap_error create_instance(ApplicationData &app_data,
                                       InstanceCreateInfo &create_info);

struct QueueFamilyIndices {
public:
  std::optional<uint32_t> _graphicsFamily;
};
void find_queue_families(VkPhysicalDevice device, QueueFamilyIndices &indices);
inline bool queue_family_indices_complete(QueueFamilyIndices &indices) {
  return indices._graphicsFamily.has_value();
}
struct PhysicalDeviceSelectionInfo {
public:
  int32_t _discreteness = 1000;
  float_t _imageSizeImportance = 1.0f;
};
[[nodiscard]] int32_t
rate_device_suitability(VkPhysicalDevice device,
                        PhysicalDeviceSelectionInfo &scoring);
[[nodiscard]] ap_error
select_physical_device(ApplicationData &app_data,
                       PhysicalDeviceSelectionInfo &selection_info);

struct LogicalDeviceCreationInfo {
public:
  InstanceCreateInfo *p_instanceCreateInfo;
};
[[nodiscard]] ap_error
create_logical_device(ApplicationData &app_data,
                      LogicalDeviceCreationInfo &create_info);
} // namespace apeiron_core::vk

#endif // __VK_MAIN_HPP__
