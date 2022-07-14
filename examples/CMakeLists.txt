include_guard()

add_executable(
  simple_pendulum
  ${CMAKE_CURRENT_SOURCE_DIR}/simple_pendulum/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/simple_pendulum/simple_pendulum.cpp)
target_include_directories(simple_pendulum PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(simple_pendulum PRIVATE mujoco::mujoco)
target_compile_definitions(
  simple_pendulum
  PRIVATE MUJOCOEXT_RESOURCES_PATH="${PROJECT_SOURCE_DIR}/resources/")
