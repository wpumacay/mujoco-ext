include_guard()

# -------------------------------------
# Create a library target for the core functionality
# TODO(wilbert): move from examples to core folder in root of project

add_library(MujocoExtCore ${CMAKE_CURRENT_SOURCE_DIR}/interface/application.cpp)
target_include_directories(MujocoExtCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(MujocoExtCore PUBLIC mujoco::mujoco)
target_compile_definitions(
  MujocoExtCore
  PUBLIC MUJOCOEXT_RESOURCES_PATH="${PROJECT_SOURCE_DIR}/resources/")
if(MUJOCOEXT_BUILD_HEADLESS)
  target_compile_definitions(MujocoExtCore PUBLIC MUJOCOEXT_BUILD_HEADLESS)
else()
  target_link_libraries(MujocoExtCore PUBLIC glfw)
endif()

add_library(MujocoExt::Core ALIAS MujocoExtCore)

# -------------------------------------
# Create executables for each example in the list
set(MUJOCOEXT_EXAMPLES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/simple_pendulum/simple_pendulum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/double_pendulum/double_pendulum.cpp)

foreach(example_filepath IN LISTS MUJOCOEXT_EXAMPLES_LIST)
  get_filename_component(target_name ${example_filepath} NAME_WLE)
  add_executable(${target_name})
  target_sources(${target_name} PRIVATE ${example_filepath})
  target_link_libraries(${target_name} PRIVATE MujocoExt::Core)
endforeach()
