#include "vk_main.hpp"
// #include <vulkan/vulkan_core.h>

namespace apeiron_core::vk {
int32_t create_instance(ApplicationData &app_data,
                        InstanceCreateInfo &create_info) {
  VLOG_SCOPE_F(3, "Creating Vulkan Instance");
  // Setup VkApplicationInfo and VkInstanceCreateInfo
  VkApplicationInfo app_info{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = create_info.str_applicationName,
      .applicationVersion = create_info._version,
      .pEngineName = "apeiron",
      .engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
      .apiVersion = VK_API_VERSION_1_3,
  };

  VkInstanceCreateInfo instance_create_info{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &app_info,
  };

  // Get instance extensions
  uint32_t extension_count = 0;
  const char **extensions;

  VLOG_F(3, "Getting instance extensions");
  switch (app_data._windowType) {
  case apeiron_core::window::WindowType::SDL:
    VLOG_F(3, "Querying SDL for instance extensions");
    if (auto ret = apeiron_core::window::sdl_get_instance_extensions(
            app_data.p_SDLWindow, &extension_count, &extensions);
        ret != Errors::SUCCESS) {
      LOG_F(ERROR,
            "An error occured whilst querying SDL for VkInstance extensions "
            "(code: %d):",
            ret);
      LOG_F(ERROR, "\t%s", window::sdl_get_error());
      return ret;
    }
    break;
  case apeiron_core::window::WindowType::GLFW:
    break;
  default:
    break;
  }

  // Log extensions
  {
    std::string extensions_print = "";
    for (auto i = 0; i < extension_count; ++i) {
      extensions_print += "\t";
      extensions_print += extensions[i];
      if (i + 1 != extension_count)
        extensions_print += "\n";
    }
    LOG_F(3, "Successfully queried for [%d] VkInstance extensions: \n%s",
          extension_count, extensions_print.c_str());
  }

  // Set extensions
  instance_create_info.enabledExtensionCount = extension_count;
  instance_create_info.ppEnabledExtensionNames = extensions;

  instance_create_info.enabledLayerCount = 0; // TODO: Add debugging layers

  // Create instance
  if (auto res =
          vkCreateInstance(&instance_create_info, nullptr, &app_data._instance);
      res != VK_SUCCESS) {
    LOG_F(ERROR, "An error occured when creating VkInstance (code[vk]: %d)",
          res);
    return Errors::VULKAN_FAILED_TO_CREATE_INSTANCE;
  }

  return Errors::SUCCESS;
}
} // namespace apeiron_core::vk
