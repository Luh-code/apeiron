cmake_minimum_required(VERSION 3.12)

project(
  apeiron-core
  VERSION 1.0
  LANGUAGES CXX
)
include(CTest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(DGLFW_USE_WAYLAND ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../bin/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../bin/lib)

add_subdirectory(${PROJECT_SOURCE_DIR}/deps/sdl2)
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/glfw)
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/glm)

add_library(apeiron-core)
target_sources(apeiron-core
  PRIVATE
  ${PROJECT_SOURCE_DIR}/src/window/sdl/sdl_main.cpp
  ${PROJECT_SOURCE_DIR}/src/window/sdl/sdl_main.hpp
  PUBLIC
  ${PROJECT_SOURCE_DIR}/src/include/apeiron_core/apeiron_core.hpp
  INTERFACE
)
target_include_directories(apeiron-core PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_libraries(apeiron-core ${SDL2_LIBRARIES})

enable_testing()
add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
