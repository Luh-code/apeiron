cmake_minimum_required(VERSION 3.12)

project(
  apeiron-core
  VERSION 1.0
  LANGUAGES CXX
)
include(CTest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../bin/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../bin/lib)

set(APEIRON_CORE_INCLUDES ${PROJECT_SOURCE_DIR}/src/include/)

add_library(apeiron-core)
target_sources(apeiron-core
  PRIVATE
  ${PROJECT_SOURCE_DIR}/src/window/sdl/sdl_main.cpp
  ${PROJECT_SOURCE_DIR}/src/window/sdl/sdl_main.hpp
  PUBLIC
  ${PROJECT_SOURCE_DIR}/src/include/apeiron_core/apeiron_core.hpp
  INTERFACE
)

enable_testing()
add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
