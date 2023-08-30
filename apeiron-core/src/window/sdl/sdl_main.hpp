#ifndef __SDL_MAIN_HPP__
#define __SDL_MAIN_HPP__

#include "../../errors.hpp"
#include "../window.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

#include <cstdint>
#include <iostream>
#include <string>

namespace apeiron_core::window {

int32_t sdl_init(int32_t flags);
int32_t sdl_create_window(SDL_Window *&window, WindowCreateInfo &create_info);
int32_t sdl_get_surface(SDL_Surface *&surface, SDL_Window *window);
int32_t sdl_deininit();

const char *sdl_get_error();

int32_t sdl_test(uint32_t millis);
} // namespace apeiron_core::window

#endif // __SDL_MAIN_HPP__
