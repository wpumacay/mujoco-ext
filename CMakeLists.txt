cmake_minimum_required(VERSION 3.15)
# ~~~
# These sources helped a lot:
# * CMake imported targets: https://tinyurl.com/rrt9v5u8
# * Fix for RPATH issue (build): https://tinyurl.com/wfnffk8e
# ~~~

project(
  MuJoCoCppExt
  VERSION 0.1.0
  DESCRIPTION
    "Some C++ extensions and samples built on top of the MuJoCo physics engine"
  HOMEPAGE_URL "https://github.com/wpumacay/mujoco-ext"
  LANGUAGES C CXX)

# @todo(wilbert): do we need these for installation?
# ~~~
# set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
# set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
# set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# ~~~

# Make sure we generate compile_commands.json for our IDEs|Editors to use
message(STATUS "Generating compile commands for static analysis tools")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include our cmake folder for modules and helper cmake scripts
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Expose some options to the user
option(MUJOCO_EXT_PREFER_LOCAL
       "Hints findpackage to look for MuJoCo in our system first" ON)
option(MUJOCO_EXT_BUILD_MUJOCO_DEMOS
       "Build the samples provided by MuJoCo itself (e.g. simulate, basic)" ON)
option(MUJOCO_EXT_BUILD_EXAMPLES
       "Build the examples provided with this set of extensions" ON)

find_package(Mujoco REQUIRED)

# ~~~
# # Build the simulation example
# add_executable(
#   AppSimulate
#   ${CMAKE_CURRENT_SOURCE_DIR}/third_party/mujoco/source/include/uitools.c
#   ${CMAKE_CURRENT_SOURCE_DIR}/third_party/mujoco/source/sample/simulate.cc)
# target_link_libraries(AppSimulate PRIVATE mujoco::glew mujoco::glfw
#                                           mujoco::mujoco)
#
# # Build samples folder
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/sample/basic)
# ~~~
