include_guard()

# -------------------------------------
# Create executables for each example in the list
set(MUJOCOEXT_EXAMPLES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/simple_pendulum/simple_pendulum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/double_pendulum/double_pendulum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cart_pole/cart_pole.cpp)

foreach(example_filepath IN LISTS MUJOCOEXT_EXAMPLES_LIST)
  get_filename_component(target_name ${example_filepath} NAME_WLE)
  add_executable(${target_name})
  target_sources(${target_name} PRIVATE ${example_filepath})
  target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(${target_name} PRIVATE MujocoExt::Core)
endforeach()
