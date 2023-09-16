#include "app.hpp"
#include <vector>

namespace apeiron_core {
int32_t init_window(window::WindowCreateInfo *create_info,
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

int32_t init_SDL_window(window::WindowCreateInfo *create_info,
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

  // SDL_Delay(5000);

  return Errors::SUCCESS;
}

int32_t init_vulkan(vk::InstanceCreateInfo *instance_create_info,
                    ApplicationData &app_data) {
  // vk::InstanceCreateInfo instance_create_info{
  //     .str_applicationName = "SDL Vulkan Window",
  //     ._version = VK_MAKE_API_VERSION(0, 1, 0, 0),
  //     .v_extensions =
  //         std::vector<const char *>{
  //             "VK_KHR_surface",
  //             "BLA_BLA_doesnt_exist",
  //         },
  //     .b_queryForExtensions = true,
  //     .v_layers = std::vector<const char *>(0),
  // };
  if (auto ret = vk::create_instance(app_data, *instance_create_info);
      ret != Errors::SUCCESS) {
    return ret;
  }
  return Errors::SUCCESS;
}

int32_t normal_init(ApplicationCreateInfo *create_info,
                    ApplicationData &app_data) {
  if (!create_info->p_windowCreateInfo) {
    LOG_F(ERROR, "p_windowCreateInfo is not defined!");
    return Errors::WINDOW_CREATE_INFO_NOT_DEFINED;
  }
  if (auto ret = init_window(create_info->p_windowCreateInfo, app_data);
      ret != Errors::SUCCESS) {
    return ret;
  }
  if (auto ret = init_vulkan(create_info->p_instanceCreateInfo, app_data);
      ret != Errors::SUCCESS) {
    return ret;
  }
  return Errors::SUCCESS;
}

int32_t normal_cleanup(int32_t, ApplicationData &app_data) {
  LOG_SCOPE_F(INFO, "Cleaning up application");

  // Clean up Vulkan
  VLOG_F(1, "Cleaning up Vulkan");
  // Clean up Instance
  if (app_data._instance) {
    vkDestroyInstance(app_data._instance, nullptr);
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
