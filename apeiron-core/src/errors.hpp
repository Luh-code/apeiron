#ifndef __ERRORS_HPP__
#define __ERRORS_HPP__

#include <cstdint>

namespace apeiron_core::errors {
using error_code = const int32_t;

error_code SUCCESS = 0;
error_code SDL_FAILED_WINDOW_CREATION = -1;
error_code SDL_FAILED_TO_GET_SURFACE = -2;
error_code SDL_ALREADY_INITIALIZED = -3;
error_code SDL_NOT_INITIALIZED = -4;
error_code SDL_FAILED_INITIALIZATION = -5;
} // namespace apeiron_core::errors

#endif // !__ERRORS_HPP__
