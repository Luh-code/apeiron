#ifndef __SDL_MAIN_HPP__
#define __SDL_MAIN_HPP__

#include "../../errors.hpp"
#include "../window.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_vulkan.h>

#include <cstdint>
#include <iostream>
#include <string>

#include <loguru.hpp>

namespace apeiron_core::window {

[[nodiscard]] int32_t sdl_init(int32_t flags);
[[nodiscard]] bool sdl_initialized(int32_t flags);
[[nodiscard]] int32_t sdl_create_window(SDL_Window *&window,
                                        WindowCreateInfo &create_info);
[[nodiscard]] int32_t sdl_get_surface(SDL_Surface *&surface,
                                      SDL_Window *&window);
[[nodiscard]] int32_t sdl_destroy_window(SDL_Window *&window);
[[nodiscard]] int32_t sdl_deinit(uint32_t flags);

[[nodiscard]] int32_t sdl_get_instance_extensions(SDL_Window *window,
                                                  uint32_t *extension_count,
                                                  const char ***extensions);

const char *sdl_get_error();

[[nodiscard]] int32_t sdl_test(uint32_t millis);
} // namespace apeiron_core::window

#endif // __SDL_MAIN_HPP__
