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
};

template <typename I, typename IR, typename M, typename MR, typename C,
          typename CR>
struct App {
public:
  int32_t (*p_init)(I, IR);
  int32_t (*p_main_loop)(M, MR);
  int32_t (*p_cleanup)(C, CR);
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
inline int32_t run_app(AppBootstrap<I, IR, M, MR, C, CR> &appBootstrap) {
  LOG_SCOPE_F(INFO, "Running App");
  int32_t ret;
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

int32_t init_window(window::WindowCreateInfo *create_info,
                    ApplicationData &app_data);
int32_t init_SDL_window(window::WindowCreateInfo *create_info,
                        ApplicationData &app_data);
int32_t init_GLFW_window(window::WindowCreateInfo *create_info,
                         ApplicationData &app_data); // TODO: implement <--
int32_t init_vulkan();
int32_t normal_init(ApplicationCreateInfo *create_info,
                    ApplicationData &app_data);
int32_t headless_init(int32_t); // TODO: implement <--
int32_t normal_cleanup(int32_t, ApplicationData &app_data);
} // namespace apeiron_core
#endif // !__APP_HPP__
