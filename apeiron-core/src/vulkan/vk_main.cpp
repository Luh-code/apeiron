#include "vk_main.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>
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
  {
    bool trying_creation = true, creation_success = false, moltenVK_fix = false;
    VkResult res;
    while (trying_creation) {
      res =
          vkCreateInstance(&instance_create_info, nullptr, &app_data._instance);
      switch (res) {
      case VK_SUCCESS: {
        trying_creation = false;
        creation_success = true;
        break;
      }

        // MoltenVK fix -- untested
      case VK_ERROR_INCOMPATIBLE_DRIVER: {
        if (moltenVK_fix) {
          trying_creation = false;
          break;
        }
        moltenVK_fix = true;
        std::vector<const char *> extensions1(extension_count + 1);
        for (auto i = 0; i < extension_count; ++i) {
          extensions1.emplace_back(extensions[i]);
        }

        extensions1.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        instance_create_info.flags |=
            VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

        instance_create_info.enabledExtensionCount =
            static_cast<uint32_t>(extensions1.size());
        instance_create_info.ppEnabledExtensionNames = extensions1.data();
        break;
      }

      default: {
        trying_creation = false;
        break;
      }
      }
    }
    if (!creation_success) {
      LOG_F(ERROR, "An error occured when creating VkInstance (code[vk]: %d)",
            res);
      return Errors::VULKAN_FAILED_TO_CREATE_INSTANCE;
    }
  }

  return Errors::SUCCESS;
}
} // namespace apeiron_core::vk
