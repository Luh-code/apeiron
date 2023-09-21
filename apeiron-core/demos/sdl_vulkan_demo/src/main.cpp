#include "apeiron_core/apeiron_core.hpp"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <cstdint>
#include <limits.h>
#include <loguru.hpp>
#include <valarray>
#include <vector>
#include <vulkan/vulkan_core.h>

int32_t main_loop(int32_t test, apeiron_core::ApplicationData &app_data) {
  LOG_SCOPE_F(INFO, "Main Loop");
  bool running = false; // normally true, false for testing
  while (running == true) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        running = false;
        LOG_F(INFO, "Quitting application");
        break;
      default:
        break;
      }
    }
  }
  return test;
}

int main(int32_t argc, char *argv[]) {
  loguru::init(argc, argv);
  char log_path[PATH_MAX];
  loguru::suggest_log_path("./logs/", log_path, sizeof(log_path));
  loguru::add_file(log_path, loguru::FileMode::Truncate, loguru::Verbosity_MAX);

  apeiron_core::App<apeiron_core::ApplicationCreateInfo *,
                    apeiron_core::ApplicationData &, int32_t,
                    apeiron_core::ApplicationData &, int32_t,
                    apeiron_core::ApplicationData &>
      app{
          .p_init = apeiron_core::normal_init,
          .p_main_loop = main_loop,
          .p_cleanup = apeiron_core::normal_cleanup,
      };

  apeiron_core::window::WindowCreateInfo window_create_info{
      ._windowType = apeiron_core::window::WindowType::SDL,
      .str_title = "SDL Vulkan window",
      ._posx = SDL_WINDOWPOS_CENTERED,
      ._posy = SDL_WINDOWPOS_CENTERED,
      ._sizex = 800,
      ._sizey = 600,
  };
  apeiron_core::vk::DebugMessengerCreateInfo debug_messenger_create_info{
      ._severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      ._types = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .p_callback = apeiron_core::vk::debug_callback,
  };
  apeiron_core::vk::InstanceCreateInfo instance_create_info{
      .str_applicationName = "SDL Vulkan Window",
      ._version = VK_MAKE_API_VERSION(0, 1, 0, 0),
      .v_extensions = std::vector<const char *>(0),
      .b_queryForExtensions = true,
      .v_layers = std::vector<const char *>(0),
      .p_debugMessengerCreateInfo = &debug_messenger_create_info,
  };
  apeiron_core::vk::PhysicalDeviceSelectionInfo physical_device_selection_info{

  };
  apeiron_core::vk::LogicalDeviceCreationInfo logical_device_create_info{
      .p_instanceCreateInfo = &instance_create_info,
  };
  apeiron_core::ApplicationCreateInfo create_info{
      .p_windowCreateInfo = &window_create_info,
      .p_instanceCreateInfo = &instance_create_info,
      .p_debugMessengerCreateInfo = &debug_messenger_create_info,
      .p_physicalDeviceSelectionInfo = &physical_device_selection_info,
      .p_logicalDeviceCreateInfo = &logical_device_create_info,
  };
  apeiron_core::ApplicationData app_data{
      .p_allocator = nullptr,
  };
  int32_t m = 0, c = 0;
  apeiron_core::AppBootstrap<apeiron_core::ApplicationCreateInfo *,
                             apeiron_core::ApplicationData &, int32_t,
                             apeiron_core::ApplicationData &, int32_t,
                             apeiron_core::ApplicationData &>
      app_bootstrap{
          .p_app = &app,

          ._i = &create_info,
          ._ir = app_data,
          ._m = m,
          ._mr = app_data,
          ._c = c,
          ._cr = app_data,
      };
  if (auto ret = apeiron_core::run_app(app_bootstrap);
      ret != apeiron_core::Errors::SUCCESS) {
    LOG_F(ERROR, "An error occured whilst running app! (code: %d)", ret);
    return -1;
  }

  return 0;
}
