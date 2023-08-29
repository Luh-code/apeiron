cmake_minimum_required(VERSION 3.1)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/demo)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/demo/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/demo/lib)

file(GLOB srcFiles
  ./src/*.h
  ./src/**/*.h
  ./src/*.hpp
  ./src/**/*.hpp
  ./src/*.c
  ./src/**/*.c
  ./src/*.cpp
  ./src/**/*.cpp
)

add_executable(sdl_window ${srcFiles})
target_include_directories(sdl_window PUBLIC ${APEIRON_CORE_INCLUDES})
target_link_libraries(sdl_window PUBLIC apeiron-core)

