cmake_minimum_required(VERSION 3.15)

project(
  MuJoCoExt
  VERSION 0.1.1
  DESCRIPTION "C++ extensions built on top of the MuJoCo physics engine"
  HOMEPAGE_URL "https://github.com/wpumacay/mujoco-ext"
  LANGUAGES C CXX)

option(MUJOCOEXT_BUILD_EXAMPLES
       "Build the examples provided with this set of extensions" ON)

# Add third_party dependencies
add_subdirectory(third_party)
