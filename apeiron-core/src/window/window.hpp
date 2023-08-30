#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

namespace apeiron_core::window {

struct WindowCreateInfo {
public:
  const char *str_title;
  int _posx, _posy;
  int _sizex, _sizey;
};

enum WindowType {
  SDL,
  GLFW,
};
} // namespace apeiron_core::window

#endif // __WINDOW_HPP__
