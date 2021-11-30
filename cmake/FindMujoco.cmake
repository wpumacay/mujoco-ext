# ~~~
# Helper module used to find mujoco in your system, or fetch it if not found.
# On successfull execution, we get the following targets:
#
# * mujoco::mujoco_gl: target that wraps both mujoco-physics and gl-dependencies
# * mujoco::mujoco_nogl: target that wraps only mujoco-physics
#
# We made used of various references along the way. These are some of those:
# * mujoco-ros integration (mush_mujoco_ros):
#   - https://github.com/prl-mushr/mushr_mujoco_ros/blob/master/CMakeLists.txt
# * CMake imported targets:
#   - https://gitlab.kitware.com/cmake/community/-/wikis/doc/tutorials/Exporting-and-Importing-Targets
# * Fix for RPATH issue (build):
#   - https://stackoverflow.com/questions/49156335/cmake-imported-library-rpath
# ~~~

# Define the target version we want (fixed for now to 2.1.0, change if needed)
# @todo(wilbert): should we expose these options to the user for they to choose?
set(MUJOCO_MAJOR_VERSION 2)
set(MUJOCO_MINOR_VERSION 1)
set(MUJOCO_PATCH_VERSION 0)
set(MUJOCO_VERSION
    "${MUJOCO_MAJOR_VERSION}${MUJOCO_MINOR_VERSION}${MUJOCO_PATCH_VERSION}")
set(MUJOCO_BASE_URL "https://mujoco.org/download")

# Setup according to our target platform (only host for now, no cross-compiling)
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(MUJOCO_DEFAULT_ROOT_LOCATION "~/.mujoco/mujoco${MUJOCO_VERSION}/")
  set(MUJOCO_DEFAULT_SOURCE_URL
      "${MUJOCO_BASE_URL}/mujoco${MUJOCO_VERSION}-linux-x86_64.tar.gz")
  set(MUJOCO_DEFAULT_SOURCE_HASH
      a436ca2f4144c38b837205635bbd60ffe1162d5b44c87df22232795978d7d012)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(MUJOCO_DEFAULT_ROOT_LOCATION "~/.mujoco/mujoco${MUJOCO_VERSION}/")
  set(MUJOCO_DEFAULT_SOURCE_URL
      "${MUJOCO_BASE_URL}/mujoco${MUJOCO_VERSION}-macos-x86_64.tar.gz")
  SET(MUJOCO_DEFAULT_SOURCE_HASH
      50226f859d9d3742fa57e1a0a92d656197ec5786f75bfa50ae00eb80fae25e90)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(MUJOCO_DEFAULT_ROOT_LOCATION "C:\\.mujoco/mujoco${MUJOCO_VERSION}/")
  set(MUJOCO_DEFAULT_SOURCE_URL
      "${MUJOCO_BASE_URL}/mujoco${MUJOCO_VERSION}-windows-x86_64.zip")
  set(MUJOCO_DEFAULT_SOURCE_HASH
      c8f62eae60d28fa944e389e858b4e2b5afcec9bfac3bd8ddfbfc769d48c76f9c)
else()
  message(
    WARNING "It looks like you're using an unsupported OS: ${CMAKE_SYSTEM_NAME}"
  )
  set(Mujoco_FOUND FALSE)
  return()
endif()

# Try to first find MuJoCo in our system, using also most common env-variables
# definitions for most common setups (e.g. mujoco-py, dm_control)
set(MUJOCO_FOUND_IN_SYSTEM FALSE)
if((NOT DEFINED ENV{MUJOCO_LOCATION})
   AND (NOT DEFINED ENV{MJLIB_PATH})
   AND (NOT DEFINED ENV{MUJOCO_PY_MUJOCO_PATH}))
  # If none of this env-variables are defined, then we'll look for the default
  # path, according to the platform in use (windows, linux, darwin)
  set(MUJOCO_ROOT_LOCATION ${MUJOCO_DEFAULT_ROOT_LOCATION})
elseif(DEFINED ENV{MUJOCO_LOCATION})
  # Use the location defined as MUJOCO_LOCATION in the environment variables
  set(MUJOCO_ROOT_LOCATION $ENV{MUJOCO_LOCATION})
elseif(DEFINED ENV{MJLIB_PATH})
  # This env-variable should point to the shared library itself, so let's go up
  # some folders (assumming of course that the folder structure is consistent)
  get_filename_component(mjc_lib_path $ENV{MJLIB_PATH} DIRECTORY)
  get_filename_component(mjc_root_path ${mjc_lib_path} DIRECTORY)
  set(MUJOCO_ROOT_LOCATION ${mjc_root_path})
elseif(DEFINED ENV{MUJOCO_PY_MUJOCO_PATH})
  # This env-variable should point to the mujocoXYZ root folder, so just use it
  set(MUJOCO_ROOT_LOCATION $ENV{MUJOCO_PY_MUJOCO_PATH})
endif()

# Query our system to look for the library in the best candidate path
if(MUJOCO_EXT_PREFER_LOCAL)
  message(
    STATUS
      "FindMujoco> try finding the library locally at ${MUJOCO_ROOT_LOCATION}")
  find_library(mjc_library_any "mujoco${MUJOCO_VERSION}"
               HINTS ${MUJOCO_ROOT_LOCATION})
  if(DEFINED mjc_library_any)
    # It looks like MuJoCo was found in the candidate location
    message(
      STATUS
        "FindMujoco> Found libmujoco in the search path. Will use local then")
    set(MUJOCO_FOUND_IN_SYSTEM TRUE)
  else()
    message(
      STATUS
        "Couldn't find mujoco locally. Will try fetching from the repo instead")
  endif()
endif()

# If MuJoCo was not found in our system, fetch the release from the repo instead
if(NOT MUJOCO_FOUND_IN_SYSTEM)
  include(FetchContent)
  set(mujoco_url ${MUJOCO_DEFAULT_SOURCE_URL})
  set(mujoco_hash ${MUJOCO_DEFAULT_SOURCE_HASH})
  # cmake-format: off
  FetchContent_Declare(
    mujocolib
    URL ${mujoco_url}
    URL_HASH SHA256=${mujoco_hash}
    PREFIX "${CMAKE_SOURCE_DIR}/third_party/mujoco"
    DOWNLOAD_DIR "${CMAKE_SOURCE_DIR}/third_party/mujoco"
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/third_party/mujoco/source"
    BINARY_DIR "${CMAKE_BINARY_DIR}/third_party/mujoco/build"
    STAMP_DIR "${CMAKE_BINARY_DIR}/third_party/mujoco/stamp"
    TMP_DIR "${CMAKE_BINARY_DIR}/third_party/mujoco/tmp")
  # cmake-format: on
  FetchContent_MakeAvailable(mujocolib)

  # Set the mujoco-root-path to this source site
  set(MUJOCO_ROOT_LOCATION "${CMAKE_SOURCE_DIR}/third_party/mujoco/source")
endif()

# Make sure we're using paths consistenly (try to remove extra /-slashes)
file(TO_CMAKE_PATH "${MUJOCO_ROOT_LOCATION}" MUJOCO_ROOT_LOCATION)
message(STATUS "FindMujoco> mujoco-root-location: ${MUJOCO_ROOT_LOCATION}")

# Expose a target that we can use ##############################################
set(CMAKE_USE_PTHREADS_INIT TRUE)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

# Create a target for the MuJoCo-physics library that uses includes rendering
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(mjc_libname_with_gl "libmujoco${MUJOCO_VERSION}.so")
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(mjc_libname_with_gl "libmujoco${MUJOCO_VERSION}.dylib")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(mjc_libname_with_gl "mujoco${MUJOCO_VERSION}.dll")
endif()
add_library(mujoco_physics_with_gl SHARED IMPORTED)
set_property(
  TARGET mujoco_physics_with_gl
  PROPERTY IMPORTED_LOCATION
           "${MUJOCO_ROOT_LOCATION}/bin/${mjc_libname_with_gl}")
set_property(TARGET mujoco_physics_with_gl PROPERTY IMPORTED_NO_SONAME TRUE)
target_include_directories(mujoco_physics_with_gl
                           INTERFACE "${MUJOCO_ROOT_LOCATION}/include")
target_compile_features(mujoco_physics_with_gl INTERFACE cxx_std_11)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(mujoco_physics_with_gl INTERFACE -mavx)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(mujoco_physics_with_gl INTERFACE /arch:AVX)
endif()
target_link_libraries(mujoco_physics_with_gl INTERFACE Threads::Threads)
# ------------------------------------------------------------------------------

# Create a target for the MuJoCo physics library that doesn't include rendering
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(mjc_libname_no_gl "libmujoco${MUJOCO_VERSION}nogl.so")
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(mjc_libname_no_gl "libmujoco${MUJOCO_VERSION}nogl.dylib")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(mjc_libname_no_gl "mujoco${MUJOCO_VERSION}nogl.dll")
endif()
add_library(mujoco_physics_no_gl SHARED IMPORTED)
set_property(
  TARGET mujoco_physics_no_gl
  PROPERTY IMPORTED_LOCATION "${MUJOCO_ROOT_LOCATION}/bin/${mjc_libname_no_gl}")
target_include_directories(mujoco_physics_no_gl
                           INTERFACE "${MUJOCO_ROOT_LOCATION}/include")
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(mujoco_physics_no_gl INTERFACE -mavx)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(mujoco_physics_no_gl INTERFACE /arch:AVX)
endif()
target_link_libraries(mujoco_physics_no_gl INTERFACE Threads::Threads)
# ------------------------------------------------------------------------------

# Create a target for the provided GLFW library --------------- ----------------
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(mjc_glfw_libname "libglfw.so.3")
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(mjc_glfw_libname "libglfw.3.dylib")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(mjc_glfw_libname "glfw3.dll")
endif()
add_library(mujoco_graphics_glfw SHARED IMPORTED)
set_property(
  TARGET mujoco_graphics_glfw
  PROPERTY IMPORTED_LOCATION "${MUJOCO_ROOT_LOCATION}/bin/${mjc_glfw_libname}")
set_property(TARGET mujoco_graphics_glfw PROPERTY IMPORTED_NO_SONAME TRUE)
target_include_directories(mujoco_graphics_glfw
                           INTERFACE "${MUJOCO_ROOT_LOCATION}/include")
# ------------------------------------------------------------------------------

# Create a target for the provided GLEW library (only required if on Linux) ----
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(mjc_glew_libname "libglew.so")
  add_library(mujoco_graphics_glew SHARED IMPORTED)
  set_property(
    TARGET mujoco_graphics_glew
    PROPERTY IMPORTED_LOCATION
             "${MUJOCO_ROOT_LOCATION}/bin/${mjc_glew_libname}")
  set_property(TARGET mujoco_graphics_glew PROPERTY IMPORTED_NO_SONAME TRUE)
  target_link_libraries(mujoco_graphics_glew INTERFACE OpenGL::OpenGL
                                                       OpenGL::GLX)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  # For MacOS, glew is required but not bundled. Use from homebrew instead
  find_package(GLEW REQUIRED)
  add_library(mujoco_graphics_glew ALIAS GLEW::GLEW)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
  # For Windows, just create a dummy target (renderer uses legacy-opengl calls)
  add_library(mujoco_graphics_glew INTERFACE)
endif()
# ------------------------------------------------------------------------------

# Create the target mujoco::mujoco_gl (version includes graphics enabled) ------
add_library(mujoco_gl INTERFACE)
target_link_libraries(
  mujoco_gl INTERFACE mujoco_physics_with_gl mujoco_graphics_glfw
                      mujoco_graphics_glew)
add_library(mujoco::mujoco_gl ALIAS mujoco_gl)
# ------------------------------------------------------------------------------

# Create the target mujoco::mujoco_nogl (version without graphics) -------------
add_library(mujoco_nogl INTERFACE)
target_link_libraries(mujoco_nogl INTERFACE mujoco_physics_no_gl)
add_library(mujoco::mujoco_nogl ALIAS mujoco_gl)
# ------------------------------------------------------------------------------

# ##############################################################################

# Notify that we found the library
message(
  STATUS "FindMujoco> successfully created imported targets (both gl and no-gl)"
)
set(Mujoco_FOUND TRUE)

# Configures the build steps for the provided MuJoCo demos (from tar|zip file)
macro(MjcConfigureDemos)
  set(one_value_args "ROOT_LOCATION")
  cmake_parse_arguments(MJC "" "${one_value_args}" "" ${ARGN})

  add_executable(mjc_demo_testxml "${MJC_ROOT_LOCATION}/sample/testxml.cc")
  target_link_libraries(mjc_demo_testxml PRIVATE mujoco::mujoco_nogl)

  add_executable(mjc_demo_testspeed "${MJC_ROOT_LOCATION}/sample/testspeed.cc")
  target_link_libraries(mjc_demo_testspeed PRIVATE mujoco::mujoco_nogl)

  add_executable(mjc_demo_compile "${MJC_ROOT_LOCATION}/sample/compile.cc")
  target_link_libraries(mjc_demo_compile PRIVATE mujoco::mujoco_nogl)

  find_package(OpenMP)
  if(OpenMP_CXX_FOUND)
    add_executable(mjc_demo_derivative
                   "${MJC_ROOT_LOCATION}/sample/derivative.cc")
    target_link_libraries(mjc_demo_derivative PRIVATE mujoco::mujoco_nogl
                                                      OpenMP::OpenMP_CXX)
  endif()

  add_executable(mjc_demo_basic "${MJC_ROOT_LOCATION}/sample/basic.cc")
  target_link_libraries(mjc_demo_basic PRIVATE mujoco::mujoco_gl)

  add_executable(mjc_demo_simulate "${MJC_ROOT_LOCATION}/sample/simulate.cc"
                                   "${MJC_ROOT_LOCATION}/include/uitools.c")
  target_link_libraries(mjc_demo_simulate PRIVATE mujoco::mujoco_gl)
endmacro()

if(MUJOCO_EXT_BUILD_MUJOCO_DEMOS)
  MjcConfigureDemos(ROOT_LOCATION ${MUJOCO_ROOT_LOCATION})
endif()
