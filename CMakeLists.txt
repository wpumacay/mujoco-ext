cmake_minimum_required(VERSION 3.15)

project(
  MuJoCoCppExt
  VERSION 0.1.0
  DESCRIPTION
    "Some C++ extensions and samples built on top of the MuJoCo physics engine"
  HOMEPAGE_URL "https://github.com/wpumacay/mujoco-ext"
  LANGUAGES C CXX)

# Make sure we generate compile_commands.json for our IDEs|Editors to use
message(STATUS "Generating compile commands for static analysis tools")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Expose some options to the user
option(MUJOCO_EXT_PREFER_LOCAL
       "Hints findpackage to look for MuJoCo in our system first" OFF)
option(MUJOCO_EXT_BUILD_MUJOCO_DEMOS
       "Build the samples provided by MuJoCo itself (e.g. simulate, basic)" ON)
option(MUJOCO_EXT_BUILD_EXAMPLES
       "Build the examples provided with this set of extensions" ON)

# Include our cmake folder for modules and helper cmake scripts
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Include MuJoCo (configures imported targets and demos)
find_package(Mujoco REQUIRED)
