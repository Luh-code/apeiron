#include "vk_main.hpp"
// #include <vulkan/vulkan_core.h>

namespace apeiron_core::vk {
int32_t create_instance(ApplicationData &app_data,
                        InstanceCreateInfo &create_info) {
  VkApplicationInfo appInfo{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = create_info.str_applicationName,
      .applicationVersion = create_info._version,
      .pEngineName = "apeiron",
      .engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
      .apiVersion = VK_API_VERSION_1_3,
  };

  VkInstanceCreateInfo create_info{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &appInfo,
  };

  uint32_t extension_count = 0;
  const char **extensions;

  switch (app_data._windowType) {
    case apeiron_core::window::WindowType
  }
}
} // namespace apeiron_core::vk
