#include "app.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>

namespace apeiron_core {
ap_error init_window(window::WindowCreateInfo *create_info,
                     ApplicationData &app_data) {
  LOG_SCOPE_F(INFO, "Initializing window");
  // Create window according to _windowType of create_info
  app_data._windowType = create_info->_windowType;
  switch (create_info->_windowType) {
  case window::WindowType::SDL:
    VLOG_F(1, "Type: SDL window");
    if (!window::sdl_initialized(0)) {
      if (auto ret = window::sdl_init(0); ret < Errors::SUCCESS) {
        VLOG_F(ERROR, "An error occured, whilst initializing SDL (code: %d)",
               ret);
        return ret;
      }
      VLOG_F(1, "Automatically initialized SDL");
    }
    return init_SDL_window(create_info, app_data);
  case window::WindowType::GLFW:
    VLOG_F(1, "Type: GLFW window");
    LOG_F(ERROR, "GLFW windows are not jet implemented!");
    // init_GLFW_window(create_info, app_data);
    return Errors::NOT_IMPLEMENTED_ERROR;
  default:
    LOG_F(ERROR, "'%d' is not a legal window type!", create_info->_windowType);
    return Errors::WINDOW_WINDOWTYPE_NOT_LEGAL;
  }
  return Errors::SUCCESS;
}

ap_error init_SDL_window(window::WindowCreateInfo *create_info,
                         ApplicationData &app_data) {
  if (auto ret = window::sdl_initialized(0); !ret) {
    LOG_F(ERROR, "SDL is not initialized!");
    return Errors::SDL_NOT_INITIALIZED;
  }

  if (auto ret = window::sdl_create_window(app_data.p_SDLWindow, *create_info);
      ret != Errors::SUCCESS) {
    return ret;
  }

  // if (auto ret =
  //         window::sdl_get_surface(app_data.p_SDLSurface,
  //         app_data.p_SDLWindow);
  //     ret != Errors::SUCCESS) {
  //   return ret;
  // }

  SDL_UpdateWindowSurface(app_data.p_SDLWindow);

  return Errors::SUCCESS;
}

ap_error init_vulkan(ApplicationCreateInfo *create_info,
                     ApplicationData &app_data) {
  if (auto ret =
          vk::create_instance(app_data, *create_info->p_instanceCreateInfo);
      ret < Errors::SUCCESS) {
    return ret;
  }
  if (create_info->p_instanceCreateInfo->b_enableValidationLayers) {
    if (auto ret = vk::setup_debug_messenger(
            app_data, *create_info->p_debugMessengerCreateInfo);
        ret < Errors::SUCCESS) {
      return ret;
    }
  }
  switch (create_info->p_windowCreateInfo->_windowType) {
  case window::WindowType::SDL:
    if (auto ret = window::sdl_create_vk_surface_khr(app_data);
        ret < Errors::SUCCESS) {
      return ret;
    }
    break;
  case window::WindowType::GLFW:
    LOG_F(ERROR, "GLFW support is not implemented yet!");
    return Errors::NOT_IMPLEMENTED_ERROR;
    break;
  default:
    LOG_F(ERROR, "'%d' is not a valid value for a _windowType",
          create_info->p_windowCreateInfo->_windowType);
    return Errors::WINDOW_WINDOWTYPE_NOT_LEGAL;
    break;
  }
  if (auto ret = vk::select_physical_device(
          app_data, *create_info->p_physicalDeviceSelectionInfo);
      ret < Errors::SUCCESS) {
    return ret;
  }
  if (auto ret = vk::create_logical_device(
          app_data, *create_info->p_logicalDeviceCreateInfo);
      ret < Errors::SUCCESS) {
    return ret;
  }

  if (create_info->b_deleteAfterUse) {
    delete create_info->p_instanceCreateInfo;
    delete create_info->p_debugMessengerCreateInfo;
    delete create_info->p_physicalDeviceSelectionInfo;
    delete create_info->p_logicalDeviceCreateInfo;
  }
  return Errors::SUCCESS;
}

ap_error normal_init(ApplicationCreateInfo *create_info,
                     ApplicationData &app_data) {
  if (!create_info->p_windowCreateInfo) {
    LOG_F(ERROR, "p_windowCreateInfo is not defined!");
    return Errors::WINDOW_CREATE_INFO_NOT_DEFINED;
  }
  if (auto ret = init_window(create_info->p_windowCreateInfo, app_data);
      ret != Errors::SUCCESS) {
    return ret;
  }
  if (auto ret = init_vulkan(create_info, app_data); ret != Errors::SUCCESS) {
    return ret;
  }
  return Errors::SUCCESS;
}

ap_error normal_cleanup(int32_t, ApplicationData &app_data) {
  LOG_SCOPE_F(INFO, "Cleaning up application");

  // Clean up Vulkan
  VLOG_F(1, "Cleaning up Vulkan");
  // Clean up Device
  if (app_data._device) {
    vkDestroyDevice(app_data._device, app_data.p_allocator);
    VLOG_F(2, "Cleaned up VkDevice");
  }
  // Clean up Debug messenger
  if (app_data._debugMessenger) {
    vk::destroy_debug_utils_messenger_ext(
        app_data._instance, app_data._debugMessenger, app_data.p_allocator);
    VLOG_F(2, "Cleaned up VkDebugUtilsMessengerEXT");
  }
  // Clean up Surface
  if (app_data._surface) {
    vkDestroySurfaceKHR(app_data._instance, app_data._surface,
                        app_data.p_allocator);
    VLOG_F(2, "Cleaned up VkSurfaceKHR");
  }
  // Clean up Instance
  if (app_data._instance) {
    vkDestroyInstance(app_data._instance, app_data.p_allocator);
    VLOG_F(2, "Cleaned up VkInstance");
  }

  // Clean up window
  switch (app_data._windowType) {
  case window::WindowType::SDL: {
    VLOG_SCOPE_F(1, "Cleaning up SDL");
    if (auto ret = window::sdl_destroy_window(app_data.p_SDLWindow);
        ret != Errors::SUCCESS) {
      LOG_F(ERROR, "An error occured whilst destroying SDL window (code:%d)",
            ret);
      return ret;
    }
    VLOG_F(2, "Successfully destroyed SDL window");
    if (auto ret = window::sdl_deinit(0); ret != Errors::SUCCESS) {
      LOG_F(ERROR, "An error occured whilst quitting SDL (code: %d)", ret);
    }
    VLOG_F(2, "Successfully quit SDL");
  } break;
  case window::WindowType::GLFW:
    VLOG_F(1, "Cleaning up GLFW window");
    break;
  default:
    LOG_F(ERROR, "'%d' is not a legal window type!", app_data._windowType);
    return Errors::WINDOW_WINDOWTYPE_NOT_LEGAL;
  }

  return Errors::SUCCESS;
}
} // namespace apeiron_core
