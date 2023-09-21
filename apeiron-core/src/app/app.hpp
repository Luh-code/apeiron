#ifndef __APP_HPP__
#define __APP_HPP__

#include "../errors.hpp"
#include "../vulkan/vk_main.hpp"
#include "../window/sdl/sdl_main.hpp"
#include "../window/window.hpp"
#include "app_data.hpp"
#include <functional>
#include <loguru.hpp>

namespace apeiron_core {

struct ApplicationCreateInfo {
public:
  window::WindowCreateInfo *p_windowCreateInfo;
  vk::InstanceCreateInfo *p_instanceCreateInfo;
  vk::DebugMessengerCreateInfo *p_debugMessengerCreateInfo;
  vk::PhysicalDeviceSelectionInfo *p_physicalDeviceSelectionInfo;
  vk::LogicalDeviceCreationInfo *p_logicalDeviceCreateInfo;
};

template <typename I, typename IR, typename M, typename MR, typename C,
          typename CR>
struct App {
public:
  ap_error (*p_init)(I, IR);
  ap_error (*p_main_loop)(M, MR);
  ap_error (*p_cleanup)(C, CR);
};
template <typename I, typename IR, typename M, typename MR, typename C,
          typename CR>
struct AppBootstrap {
public:
  App<I, IR, M, MR, C, CR> *p_app;

  I _i;
  IR _ir;
  M _m;
  MR _mr;
  C _c;
  CR _cr;
};
template <typename I, typename IR, typename M, typename MR, typename C,
          typename CR>
[[nodiscard]] inline ap_error
run_app(AppBootstrap<I, IR, M, MR, C, CR> &appBootstrap) {
  LOG_SCOPE_F(INFO, "Running App");
  ap_error ret;
  if (ret = appBootstrap.p_app->p_init(appBootstrap._i, appBootstrap._ir);
      ret < Errors::SUCCESS) {
    return ret;
  }
  if (ret = appBootstrap.p_app->p_main_loop(appBootstrap._m, appBootstrap._mr);
      ret < Errors::SUCCESS) {
    return ret;
  }
  if (ret = appBootstrap.p_app->p_cleanup(appBootstrap._c, appBootstrap._cr);
      ret < Errors::SUCCESS) {
    return ret;
  }
  return ret;
}

[[nodiscard]] ap_error init_window(window::WindowCreateInfo *create_info,
                                   ApplicationData &app_data);
[[nodiscard]] ap_error init_SDL_window(window::WindowCreateInfo *create_info,
                                       ApplicationData &app_data);
[[nodiscard]] ap_error
init_GLFW_window(window::WindowCreateInfo *create_info,
                 ApplicationData &app_data); // TODO: implement <--
[[nodiscard]] ap_error init_vulkan(ApplicationCreateInfo *create_info,
                                   ApplicationData &app_data);
[[nodiscard]] ap_error normal_init(ApplicationCreateInfo *create_info,
                                   ApplicationData &app_data);
[[nodiscard]] ap_error headless_init(int32_t); // TODO: implement <--
[[nodiscard]] ap_error normal_cleanup(int32_t, ApplicationData &app_data);
} // namespace apeiron_core
#endif // !__APP_HPP__
