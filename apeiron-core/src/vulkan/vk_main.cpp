#include "vk_main.hpp"
#include <optional>
#include <vulkan/vulkan_core.h>

namespace apeiron_core::vk {
void populate_debug_vk_messenger_create_info(
    VkDebugUtilsMessengerCreateInfoEXT &out, DebugMessengerCreateInfo &in) {
  out = {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageSeverity = in._severity,
      .messageType = in._types,
      .pfnUserCallback = in.p_callback,
      .pUserData = nullptr,
  };
}

VkResult create_debug_utils_messenger_ext(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *create_info,
    const VkAllocationCallbacks *allocator,
    VkDebugUtilsMessengerEXT *debug_messenger) {

  // Fetch function pointer to vkCreateDebugUtilsMessengerEXT
  const char *func_str = "vkCreateDebugUtilsMessengerEXT";
  auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(instance, func_str));
  if (!func) {
    LOG_F(ERROR, "Could not get function '%s'", func_str);
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }

  // Run the fetched function
  return func(instance, create_info, allocator, debug_messenger);
}

void destroy_debug_utils_messenger_ext(VkInstance instance,
                                       VkDebugUtilsMessengerEXT debug_messenger,
                                       const VkAllocationCallbacks *allocator) {

  // Fetch function pointer to vkCreateDebugUtilsMessengerEXT
  const char *func_str = "vkDestroyDebugUtilsMessengerEXT";
  auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(instance, func_str));
  if (!func) {
    LOG_F(ERROR, "Could not get function '%s'", func_str);
    return;
  }

  // Run the fetched function
  func(instance, debug_messenger, allocator);
}

ap_error setup_debug_messenger(ApplicationData &app_data,
                               DebugMessengerCreateInfo &create_info) {
  VLOG_SCOPE_F(1, "Setting up Vulkan debug messenger");

  // Check if instance is initialized
  if (!app_data._instance) {
    LOG_F(ERROR,
          "Cannot create Vulkan message callback before creating VkInstance!");
    return Errors::VULKAN_INSTANCE_NOT_INITIALIZED;
  }

  // Create create info
  VkDebugUtilsMessengerCreateInfoEXT dbm_create_info;
  populate_debug_vk_messenger_create_info(dbm_create_info, create_info);

  // Try to create debug messenger
  if (auto ret = create_debug_utils_messenger_ext(
          app_data._instance, &dbm_create_info, app_data.p_allocator,
          &app_data._debugMessenger);
      ret != VK_SUCCESS) {
    LOG_F(ERROR, "Failed to set up Vulkan debug messenger!");
    return Errors::VULKAN_FAILED_TO_SET_UP_DEBUG_MESSENGER;
  }

  VLOG_F(1, "Set up Vulkan debug messenger successfully");

  return Errors::SUCCESS;
}

ap_error create_instance(ApplicationData &app_data,
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
      return Errors::NOT_IMPLEMENTED_ERROR;
    default:
      return Errors::INVALID_PATH_ERROR;
    }
  }

  // Add queried extensions to v_extensions
  for (auto i = 0; i < extension_count; ++i) {
    create_info.v_extensions.push_back(extensions[i]);
  }
  delete[] extensions;

  VkDebugUtilsMessengerCreateInfoEXT messenger_create_info;
  if (create_info.b_enableValidationLayers) {
    create_info.v_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    populate_debug_vk_messenger_create_info(
        messenger_create_info, *create_info.p_debugMessengerCreateInfo);
    instance_create_info.pNext = &messenger_create_info;
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

void find_queue_families(VkPhysicalDevice device, QueueFamilyIndices &indices,
                         VkSurfaceKHR &surface) {
  indices = {};

  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                           nullptr);

  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                           queue_families.data());

  for (int32_t i = 0; i < queue_family_count; ++i) {
    const auto &queue_family = queue_families[i];

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
    // Set _graphicsFamily
    if (queue_family.queueFlags &
        VK_QUEUE_GRAPHICS_BIT) {                  // If grapics are supported
      if (!indices._graphicsFamily.has_value()) { // Set if empty
        indices._graphicsFamily = i;
      } else if (presentSupport) { // Set if not empty, but supports present
                                   // (performance boost if the same queue is
                                   // used)
        indices._graphicsFamily = i;
      }
    }

    // Set _presentFamily
    if (presentSupport) {                        // If present is supported
      if (!indices._presentFamily.has_value()) { // Set if empty
        indices._presentFamily = i;
      } else if (indices._graphicsFamily.has_value() &&
                 indices._graphicsFamily.value() ==
                     i) { // Set if not empty, but supports graphics
        indices._presentFamily = i;
      }
    }

    if (queue_family_indices_ideal(indices)) {
      VLOG_F(2, "Found ideal queue configuration");
      break;
    }
  }
}

bool check_device_extension_support(
    VkPhysicalDevice device, PhysicalDeviceSelectionInfo selection_info) {
  uint32_t extension_count;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count,
                                       nullptr);

  std::vector<VkExtensionProperties> available_extensions(extension_count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count,
                                       available_extensions.data());

  std::set<std::string> required_extensions(selection_info.v_extensions.begin(),
                                            selection_info.v_extensions.end());

  VkPhysicalDeviceProperties device_properties;
  vkGetPhysicalDeviceProperties(device, &device_properties);

  VLOG_F(4, "All [%lu] available device extensions of '%s':",
         available_extensions.size(), device_properties.deviceName);

  for (const auto &extension : available_extensions) {
    required_extensions.erase(extension.extensionName);
    VLOG_F(4, " - %s", extension.extensionName);
  }

  return required_extensions.empty();
}

void query_swap_chain_support(VkPhysicalDevice &device, VkSurfaceKHR &surface,
                              SwapChainSupportDetails &swap_chain_support) {
  swap_chain_support = {};

  // Get capabilities of surface
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                            &swap_chain_support._capabilities);

  // Get available surface formats
  uint32_t format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);
  if (format_count != 0) {
    swap_chain_support.v_formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count,
                                         swap_chain_support.v_formats.data());
  }

  // Get available present modes
  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
                                            &present_mode_count, nullptr);
  if (present_mode_count != 0) {
    swap_chain_support.v_presentModes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &present_mode_count,
        swap_chain_support.v_presentModes.data());
  }
}

int32_t rate_device_suitability(ApplicationData &app_data,
                                VkPhysicalDevice device,
                                PhysicalDeviceSelectionInfo &scoring) {
  VkPhysicalDeviceProperties device_properties;
  VkPhysicalDeviceFeatures device_features;
  vkGetPhysicalDeviceProperties(device, &device_properties);
  vkGetPhysicalDeviceFeatures(device, &device_features);

  int32_t score = 0;

  // Check for queue families present
  QueueFamilyIndices indices;
  find_queue_families(device, indices, app_data._surface);

  // auto indices_complete = [indices]() -> bool {
  //   return indices._graphicsFamily.has_value();
  // };

  // Check for required functionality
  if (!device_features.geometryShader) {
    VLOG_F(3, "Geometry shader required but not supported");
    return score;
  }
  if (!queue_family_indices_complete(indices)) {
    VLOG_F(3, "Not all required queues have been found");
    return score;
  }
  if (!check_device_extension_support(device, scoring)) {
    VLOG_F(3, "Device doesn't support all required extensions");
    return score;
  }
  SwapChainSupportDetails swap_chain_support;
  query_swap_chain_support(device, app_data._surface, swap_chain_support);

  if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    score += scoring._discreteness;
  }

  score += device_properties.limits.maxImageDimension2D *
           scoring._imageSizeImportance;

  if (queue_family_indices_ideal(indices)) {
    score += scoring._queueIdealness;
  }

  return score;
}

ap_error select_physical_device(ApplicationData &app_data,
                                PhysicalDeviceSelectionInfo &selection_info) {
  VLOG_SCOPE_F(1, "Selecting physical device");
  // Get a list of all GPUs supporting Vulkan
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(app_data._instance, &device_count, nullptr);

  if (device_count == 0) {
    LOG_F(ERROR, "Failed to find GPUs with Vulkan support!");
    return Errors::VULKAN_GPU_NOT_SUPPORTED;
  }

  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(app_data._instance, &device_count, devices.data());

  // Rank Devices
  std::multimap<int32_t, VkPhysicalDevice> candidates;

  for (const auto &device : devices) {
    int32_t score = rate_device_suitability(app_data, device, selection_info);
    candidates.insert(std::make_pair(score, device));
  }

  // Log Devices with ranks
  VLOG_F(2, "Found [%ld] devices:", candidates.size());
  for (const auto &p : candidates) {
    VkPhysicalDeviceProperties device_properties;
    vkGetPhysicalDeviceProperties(p.second, &device_properties);
    VLOG_F(2, "\t%s", device_properties.deviceName);
    VLOG_F(2, "\t - device_id: %u", device_properties.deviceID);
    VLOG_F(2, "\t - vendor_id: %u", device_properties.vendorID);
    VLOG_F(2, "\t - api_ver: %u", device_properties.apiVersion);
    const char *type_str;
    switch (device_properties.deviceType) {
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
      type_str = "VK_PHYSICAL_DEVICE_TYPE_CPU";
      break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
      type_str = "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU";
      break;
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
      type_str = "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU";
      break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
      type_str = "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU";
      break;
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
      type_str = "VK_PHYSICAL_DEVICE_TYPE_OTHER";
      break;
    default:
      type_str = "unknown";
      break;
    }
    VLOG_F(2, "\t - type: %s", type_str);
    VLOG_F(2, "\t - driver_ver: %u", device_properties.driverVersion);
    VLOG_F(2, "\tSCORE=%d", p.first);
  }

  // Set physical device if a suitable candidate was found
  if (candidates.rbegin()->first <= 0) {
    LOG_F(ERROR, "Failed to find GPU with required features!");
    return Errors::VULKAN_GPU_NOT_SUPPORTED;
  }
  VkPhysicalDeviceProperties device_properties;
  vkGetPhysicalDeviceProperties(candidates.rbegin()->second,
                                &device_properties);

  app_data._physicalDevice = candidates.rbegin()->second;
  LOG_F(INFO, "Selected '%s'", device_properties.deviceName);

  QueueFamilyIndices indices;
  find_queue_families(app_data._physicalDevice, indices, app_data._surface);
  // app_data._queueFamilyIndices = indices;
  VLOG_F(2, " - graphics queue: %u", indices._graphicsFamily.value());

  return Errors::SUCCESS;
}

ap_error create_logical_device(ApplicationData &app_data,
                               LogicalDeviceCreationInfo &create_info) {
  VLOG_SCOPE_F(1, "Creating logical device");

  // Query for queue indices and create queue create infos
  QueueFamilyIndices indices;
  find_queue_families(app_data._physicalDevice, indices, app_data._surface);

  // For now just the graphics queue - this WILL change
  std::vector<VkDeviceQueueCreateInfo> queue_create_infos{};
  std::set<uint32_t> unique_queue_families = {indices._graphicsFamily.value(),
                                              indices._presentFamily.value()};
  float queue_priority = 1.0f; // 0.0f - 1.0f, impacts command buffer scheduling
  for (uint32_t queue_family : unique_queue_families) {
    bool same = false;
    for (auto info : queue_create_infos) {
      if (info.queueFamilyIndex == queue_family) {
        same = true;
        break;
      }
    }
    if (same) {
      continue;
    }
    queue_create_infos.push_back({
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queue_family,
        .queueCount = 1,
        .pQueuePriorities = &queue_priority,
    });
  }

  VkPhysicalDeviceFeatures device_features{};

  VkDeviceCreateInfo device_create_info{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size()),
      .pQueueCreateInfos = queue_create_infos.data(),
      .pEnabledFeatures = &device_features,
  };

  device_create_info.enabledExtensionCount =
      static_cast<uint32_t>(create_info.v_extensions.size());
  device_create_info.ppEnabledExtensionNames = create_info.v_extensions.data();

  VLOG_F(3, "Selected [%u] VkDevice extensions:",
         device_create_info.enabledExtensionCount);
  for (const auto &extension_name : create_info.v_extensions) {
    VLOG_F(3, " - %s", extension_name);
  }

  // Set device specific layers for backwards compatibility
  if (create_info.p_instanceCreateInfo->b_enableValidationLayers) {
    device_create_info.enabledLayerCount = static_cast<uint32_t>(
        create_info.p_instanceCreateInfo->v_validationLayers.size());
    device_create_info.ppEnabledLayerNames =
        create_info.p_instanceCreateInfo->v_validationLayers.data();
  } else {
    device_create_info.enabledLayerCount = 0;
  }

  if (auto res = vkCreateDevice(app_data._physicalDevice, &device_create_info,
                                app_data.p_allocator, &app_data._device);
      res != VK_SUCCESS) {
    LOG_F(ERROR, "Failed to create VkDevice: %d", res);
    return Errors::VULKAN_FAILED_TO_CREATE_DEVICE;
  }

  // Get Queue handles for all defined queues
  // Queue index 0 for the graphics queue
  vkGetDeviceQueue(app_data._device, indices._graphicsFamily.value(), 0,
                   &app_data._graphicsQueue);
  // Queue index 0 for the present queue
  vkGetDeviceQueue(app_data._device, indices._presentFamily.value(), 0,
                   &app_data._presentQueue);

  VLOG_F(2, "Successfully created VkDevice!");

  return Errors::SUCCESS;
}
} // namespace apeiron_core::vk
