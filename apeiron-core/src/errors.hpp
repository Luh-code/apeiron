#ifndef __ERRORS_HPP__
#define __ERRORS_HPP__

#include <cstdint>

namespace apeiron_core {
enum Errors : int32_t {
  SUCCESS = 0,
  SDL_FAILED_WINDOW_CREATION = -1,
  SDL_FAILED_TO_GET_SURFACE = -2,
  SDL_ALREADY_INITIALIZED = -3,
  SDL_NOT_INITIALIZED = -4,
  SDL_FAILED_INITIALIZATION = -5,
};
} // namespace apeiron_core

#endif // !__ERRORS_HPP__
