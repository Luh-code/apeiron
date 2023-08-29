#ifndef __GLFW_MAIN_HPP__
#define __GLFW_MAIN_HPP__

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANTS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

namespace apeiron_core::window {
void glfw_test();
} // namespace apeiron_core::window

#endif // __GLFW_MAIN_HPP__
