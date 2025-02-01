# Recursively find all C++ source files in src/ and subdirectories
file(GLOB_RECURSE ENGINE_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

# Debug: Print found files
message(STATUS "Source files: ${ENGINE_SOURCES}")

add_executable(engine
  ${ENGINE_SOURCES}
)

# Compiler settings
set_property(TARGET engine PROPERTY CXX_STANDARD 20)
target_compile_definitions(engine PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_ENABLE_EXPERIMENTAL)
target_compile_definitions(engine PRIVATE PROJECT_ROOT=\"${CMAKE_SOURCE_DIR}\")

# Include directories
target_include_directories(engine PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}" # src/
  "${CMAKE_CURRENT_SOURCE_DIR}/engine" # src/
  "${CMAKE_CURRENT_SOURCE_DIR}/data structures" # src/
  "${CMAKE_CURRENT_SOURCE_DIR}/data structures/svo" # src/
  "${CMAKE_CURRENT_SOURCE_DIR}/renderer" # src/
  "${CMAKE_CURRENT_SOURCE_DIR}/renderer/pipelines" # src/
  "${CMAKE_CURRENT_SOURCE_DIR}/renderer/swapchain" # src/
  "${CMAKE_CURRENT_SOURCE_DIR}/renderer/utilities" # src/
  "${PROJECT_SOURCE_DIR}/include" # Project-wide includes
  $ENV{VULKAN_SDK}/Include # Vulkan headers
  ${SDL_SOURCE_DIR}/include # SDL2 headers
)

# Link libraries
target_link_libraries(engine PUBLIC
  vk-bootstrap # From CPM
  glm::glm # From CPM
  vma # Our custom target
  stb_image # Our custom target
  SDL2::SDL2 # From CPM
  imgui # Our custom target
  fmt::fmt # From CPM
  fastgltf::fastgltf # From CPM
  Vulkan::Vulkan # System package
)

# Optional: PCH (keep if needed)
target_precompile_headers(engine PUBLIC
  <vector>
  <memory>
  <vulkan/vulkan.h>
)

# Optional: DLL copying (keep if needed)
# add_custom_command(TARGET engine POST_BUILD
# COMMAND ${CMAKE_COMMAND} -E copy_if_different
# "$<TARGET_RUNTIME_DLLS:engine>"
# "$<TARGET_FILE_DIR:engine>"
# COMMAND_EXPAND_LISTS
# )