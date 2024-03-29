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
set(MUJOCO_BUILD_EXAMPLES TRUE)
add_subdirectory(third_party/mujoco)

# -------------------------------------
# Create a library target for the core functionality
add_library(
  MujocoExtCore
  ${CMAKE_CURRENT_SOURCE_DIR}/source/core/application.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_demo.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_draw.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_tables.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_widgets.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_glfw.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_opengl3.cpp)
target_include_directories(MujocoExtCore
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(MujocoExtCore
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui)
target_link_libraries(MujocoExtCore PUBLIC mujoco::mujoco)
target_compile_definitions(
  MujocoExtCore
  PUBLIC MUJOCOEXT_RESOURCES_PATH="${PROJECT_SOURCE_DIR}/resources/")
if(MUJOCOEXT_BUILD_HEADLESS)
  target_compile_definitions(MujocoExtCore PUBLIC MUJOCOEXT_BUILD_HEADLESS)
else()
  target_link_libraries(MujocoExtCore PUBLIC glfw)
endif()
# -------------------------------------
# Create an alias for within the MujocoExt "namespace"
add_library(MujocoExt::Core ALIAS MujocoExtCore)

# Add examples to the build workflow---
add_subdirectory(examples)
