#ifndef __VK_MAIN_HPP__
#define __VK_MAIN_HPP__

#include "../app/app_data.hpp"
#include "../window/sdl/sdl_main.hpp"
#include "../window/window.hpp"

#include <cstdint>
#include <string>

#include <vulkan/vulkan.h>

#include <vector>
#include <vulkan/vulkan_core.h>

namespace apeiron_core::vk {

struct InstanceCreateInfo {
public:
  const char *str_applicationName;
  uint32_t _version;
};
int32_t create_instance(ApplicationData &app_data,
                        InstanceCreateInfo &create_info);
} // namespace apeiron_core::vk

#endif // __VK_MAIN_HPP__
