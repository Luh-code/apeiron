#ifndef __APP_HPP__
#define __APP_HPP__

#include "errors.hpp"
#include "vulkan/vk_main.hpp"
#include "window/window.hpp"
#include <cstdint>
#include <functional>

namespace apeiron_core {

struct ApplicationCreateInfo {
public:
  window::WindowCreateInfo *p_windowCreateInfo;
};

template <typename I, typename M, typename C> struct App {
public:
  int32_t (*p_init)(I);
  int32_t (*p_main_loop)(M);
  int32_t (*p_cleanup)(C);
};
template <typename I, typename M, typename C>
inline int32_t run_app(App<I, M, C> &app, I &i, M &m, C &c) {
  int32_t ret;
  if (ret = app.p_init(i); ret < Errors::SUCCESS) {
    return ret;
  }
  if (ret = app.p_main_loop(m); ret < Errors::SUCCESS) {
    return ret;
  }
  if (ret = app.p_cleanup(c); ret < Errors::SUCCESS) {
    return ret;
  }
  return ret;
}

int32_t init_window(window::WindowCreateInfo *create_info);
int32_t init_SDL_window(window::WindowCreateInfo *create_info);
int32_t
init_GLFW_window(window::WindowCreateInfo *create_info); // TODO: implement <--
int32_t init_vulkan();
int32_t normal_init(ApplicationCreateInfo *create_info);
int32_t headless_init(int32_t); // TODO: implement <--
int32_t normal_cleanup(int32_t);
} // namespace apeiron_core
#endif // !__APP_HPP__
