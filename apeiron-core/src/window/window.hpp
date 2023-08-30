#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

namespace apeiron_core::window {

struct WindowCreateInfo {
public:
  const char *title;
  int posx, posy;
  int sizex, sizey;
};
} // namespace apeiron_core::window

#endif // __WINDOW_HPP__
