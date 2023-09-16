#include "vk_main.hpp"
#include <vulkan/vulkan_core.h>
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

  if (create_info.b_enableValidationLayers) {
    create_info.v_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

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

  // Add Validation layers to layers if requested
  if (create_info.b_enableValidationLayers) {
    for (auto i = 0; i < create_info.v_validationLayers.size(); ++i) {
      create_info.v_layers.push_back(create_info.v_validationLayers[i]);
    }
  }

  // Check for doubles and layer availability
  {
    std::vector<const char *> final_layers{};

    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    std::vector<VkLayerProperties> layer_properties(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, layer_properties.data());
    VLOG_F(2, "All [%d] Available instance layers:", layer_count);
    for (auto i = 0; i < layer_count; ++i) {
      VLOG_F(2, "\t%s", layer_properties[i].layerName);
    }

    for (auto i = 0; i < create_info.v_layers.size(); ++i) {
      // Check for doubles
      bool twice = false;
      for (auto j = i + 1; j < create_info.v_layers.size(); ++j) {
        if (strcmp(create_info.v_layers[i], create_info.v_layers[j]) == 0) {
          twice = true;
          break;
        }
      }
      if (twice) {
        LOG_F(WARNING, "Tried to add VkInstance layer '%s' twice",
              create_info.v_layers[i]);
        continue;
      }

      // Check for support
      bool layer_exists = false;
      for (auto j = 0; j < layer_properties.size(); ++j) {
        if (strcmp(layer_properties[j].layerName, create_info.v_layers[i]) ==
            0) {
          layer_exists = true;
          break;
        }
      }
      if (!layer_exists) {
        LOG_F(ERROR, "Unsupported layer '%s', continuing without",
              create_info.v_layers[i]);
        continue;
      }
      final_layers.push_back(create_info.v_layers[i]);
    }
    create_info.v_layers = final_layers;
  }

  // Log layers
  {
    LOG_F(2, "Selected [%lu] VkInstance layers: ", create_info.v_layers.size());
    for (auto i = 0; i < create_info.v_layers.size(); ++i) {
      LOG_F(2, "\t%s", create_info.v_layers[i]);
    }
  }

  instance_create_info.enabledLayerCount = create_info.v_layers.size();
  instance_create_info.ppEnabledLayerNames = create_info.v_layers.data();

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

int32_t setup_debug_messenger(ApplicationData &app_data,
                              DebugMessengerCreateInfo &create_info) {
  VLOG_SCOPE_F(1, "Setting up Vulkan debug messenger");

  // Check if instance is initialized
  if (!app_data._instance) {
    LOG_F(ERROR,
          "Cannot create Vulkan message callback before creating VkInstance!");
    return Errors::VULKAN_INSTANCE_NOT_INITIALIZED;
  }

  // Create create info
  VkDebugUtilsMessengerCreateInfoEXT dbm_create_info{
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageSeverity = create_info._severity,
      .messageType = create_info._types,
      .pfnUserCallback = create_info.p_callback,
      .pUserData = nullptr,
  };

  // Fetch function pointer to vkCreateDebugUtilsMessengerEXT
  const char *func_str = "vkCreateDebugUtilsMessengerEXT";
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      app_data._instance, func_str);
  if (!func) {
    LOG_F(ERROR, "Could not get function '%s'", func_str);
    return Errors::VULKAN_FUNCTION_NOT_FOUND;
  }

  // Try to create debug messenger
  if (auto ret = func(app_data._instance, &dbm_create_info,
                      app_data.p_allocator, &app_data._debugMessenger);
      ret != VK_SUCCESS) {
    LOG_F(ERROR, "Failed to set up Vulkan debug messenger!");
    return Errors::VULKAN_FAILED_TO_SET_UP_DEBUG_MESSENGER;
  }

  VLOG_F(1, "Set up Vulkan debug messenger successfully");

  return Errors::SUCCESS;
}
} // namespace apeiron_core::vk
