#include "vk_main.hpp"
// #include <vulkan/vulkan_core.h>

namespace apeiron_core::vk {
int32_t create_instance(ApplicationData &app_data,
                        InstanceCreateInfo &create_info) {
  VLOG_SCOPE_F(1, "Creating Vulkan Instance");
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

  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
  std::vector<VkExtensionProperties> extension_properties(extension_count);
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count,
                                         extension_properties.data());
  VLOG_F(2, "All [%d] Available instance extensions:", extension_count);
  for (auto i = 0; i < extension_count; ++i) {
    VLOG_F(2, "\t%s", extension_properties[i].extensionName);
  }
  extension_count = 0;

  // Query window lib for extensions if wanted
  if (create_info.b_queryForExtensions) {
    VLOG_F(2, "Querying instance extensions");
    switch (app_data._windowType) {
    case apeiron_core::window::WindowType::SDL:
      VLOG_F(2, "Querying SDL for instance extensions");
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
  }

  // Add queried extensions to v_extensions
  for (auto i = 0; i < extension_count; ++i) {
    create_info.v_extensions.push_back(extensions[i]);
  }
  delete[] extensions;

  // Check for doubles and extension availability
  {
    std::vector<const char *> final_extensions{};
    for (auto i = 0; i < create_info.v_extensions.size(); ++i) {
      // Check for doubles
      bool twice = false;
      for (auto j = i + 1; j < create_info.v_extensions.size(); ++j) {
        if (strcmp(create_info.v_extensions[i], create_info.v_extensions[j]) ==
            0) {
          twice = true;
          break;
        }
      }
      if (twice) {
        LOG_F(WARNING, "Tried to add VkInstance extension '%s' twice",
              create_info.v_extensions[i]);
        continue;
      }

      // Check for support
      bool extension_exists = false;
      for (auto j = 0; j < extension_properties.size(); ++j) {
        if (strcmp(extension_properties[j].extensionName,
                   create_info.v_extensions[i]) == 0) {
          extension_exists = true;
          break;
        }
      }
      if (!extension_exists) {
        LOG_F(ERROR, "Unsupported extension '%s', continuing without",
              create_info.v_extensions[i]);
        continue;
      }
      final_extensions.push_back(create_info.v_extensions[i]);
    }
    create_info.v_extensions = final_extensions;
  }

  // Log extensions
  {
    LOG_F(2, "Selected [%lu] VkInstance extensions: ",
          create_info.v_extensions.size());
    for (auto i = 0; i < create_info.v_extensions.size(); ++i) {
      LOG_F(2, "\t%s", create_info.v_extensions[i]);
    }
  }

  // Set extensions
  instance_create_info.enabledExtensionCount = create_info.v_extensions.size();
  instance_create_info.ppEnabledExtensionNames =
      create_info.v_extensions.data();

  instance_create_info.enabledLayerCount = 0; // TODO: Add validation layers

  // Create instance
  {
    bool trying_creation = true, creation_success = false, moltenVK_fix = false;
    VkResult res;
    while (trying_creation) {
      res =
          vkCreateInstance(&instance_create_info, nullptr, &app_data._instance);
      switch (res) {
      case VK_SUCCESS: {
        LOG_F(1, "Created VkInstance successfully");
        trying_creation = false;
        creation_success = true;
        break;
      }

        // MoltenVK fix -- untested
      case VK_ERROR_INCOMPATIBLE_DRIVER: {
        LOG_F(1, "Applying MoltenVK/Incompatible Driver fix");
        if (moltenVK_fix) {
          trying_creation = false;
          break;
        }
        moltenVK_fix = true;

        create_info.v_extensions.push_back(
            VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        instance_create_info.flags |=
            VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

        instance_create_info.enabledExtensionCount =
            create_info.v_extensions.size();
        instance_create_info.ppEnabledExtensionNames =
            create_info.v_extensions.data();
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
