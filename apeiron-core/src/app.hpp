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
  window::WindowType _windowType;
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
  if (ret = app.p_init(i); ret < errors::SUCCESS) {
    return ret;
  }
  if (ret = app.p_main_loop(m); ret < errors::SUCCESS) {
    return ret;
  }
  if (ret = app.p_cleanup(c); ret < errors::SUCCESS) {
    return ret;
  }
  return ret;
}

int32_t normal_init(int32_t);
int32_t normal_cleanup(int32_t);
} // namespace apeiron_core
#endif // !__APP_HPP__
