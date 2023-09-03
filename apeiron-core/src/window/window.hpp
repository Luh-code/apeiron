#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#include <cstdint>

namespace apeiron_core::window {

enum WindowType : int32_t {
  SDL = 0,
  GLFW = 1,
};

struct WindowCreateInfo {
public:
  int32_t _windowType;
  const char *str_title;
  int _posx, _posy;
  int _sizex, _sizey;
};

} // namespace apeiron_core::window

#endif // __WINDOW_HPP__
