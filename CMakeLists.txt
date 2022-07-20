cmake_minimum_required(VERSION 3.15)

project(
  MuJoCoExt
  VERSION 0.1.1
  DESCRIPTION "C++ extensions built on top of the MuJoCo physics engine"
  HOMEPAGE_URL "https://github.com/wpumacay/mujoco-ext"
  LANGUAGES C CXX)

option(MUJOCOEXT_BUILD_EXAMPLES
       "Build the examples provided with this set of extensions" ON)
option(MUJOCOEXT_BUILD_HEADLESS "Build without glfw (running on headless mode)"
       OFF)

# Export compile_commands.json (required for linting)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add third_party dependencies --------
set(MUJOCO_BUILD_TESTS FALSE)
set(MUJOCO_BUILD_EXAMPLES FALSE)
add_subdirectory(third_party/mujoco)

# Add examples to the build workflow---
add_subdirectory(examples)
