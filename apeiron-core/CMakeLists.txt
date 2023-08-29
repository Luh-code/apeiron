cmake_minimum_required(VERSION 3.7)

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

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

find_package(Vulkan REQUIRED FATAL_ERROR)
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/sdl2 EXCLUDE_FROM_ALL)
# find_package(SDL2 REQUIRED)
set(DGLFW_USE_WAYLAND ON)
option(DGLFW_USE_WAYLAND ON)
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/glfw)
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/glm)

set(APEIRON_CORE_INCLUDES ${PROJECT_SOURCE_DIR}/src/include)

add_library(apeiron-core)
target_sources(apeiron-core
  PRIVATE
  ${PROJECT_SOURCE_DIR}/src/window/sdl/sdl_main.cpp
  ${PROJECT_SOURCE_DIR}/src/window/sdl/sdl_main.hpp
  ${PROJECT_SOURCE_DIR}/src/window/glfw/glfw_main.cpp
  ${PROJECT_SOURCE_DIR}/src/window/glfw/glfw_main.hpp
  PUBLIC
  ${PROJECT_SOURCE_DIR}/src/include/apeiron_core/apeiron_core.hpp
  INTERFACE
)
# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(mygame PRIVATE SDL2::SDL2main)
endif()
target_include_directories(apeiron-core
  PUBLIC
  # ${PROJECT_SOURCE_DIR}/deps/sdl2/include
  # ${PROJECT_SOURCE_DIR}/deps/
  # ${PROJECT_SOURCE_DIR}/
  # ${SDL2_INCLUDE_DIRS}
  # ${GLFW_INCLUDE_DIRS}
  # ${GLM_INCLUDE_DIRS}
)
target_link_libraries(apeiron-core
  SDL2
  glfw
  glm
  Vulkan::Vulkan
)

add_subdirectory(demos)

enable_testing()
add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
