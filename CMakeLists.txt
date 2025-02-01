cmake_minimum_required(VERSION 3.8)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
project("VoltexEngine")

# ========== Dependencies ==========
include(cmake/CPM.cmake)

# Vulkan
find_package(Vulkan REQUIRED)

# GLM (header-only)
CPMAddPackage("gh:g-truc/glm#1.0.1")

# vk-bootstrap
CPMAddPackage(
  NAME vk-bootstrap
  GITHUB_REPOSITORY charles-lunarg/vk-bootstrap
  VERSION 0.7
  OPTIONS
  "VK_BOOTSTRAP_SAMPLES OFF"
)

# Vulkan Memory Allocator (header-only)
CPMAddPackage(
  NAME VulkanMemoryAllocator
  GITHUB_REPOSITORY GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
  VERSION 3.2.0
  DOWNLOAD_ONLY YES
)

if(VulkanMemoryAllocator_ADDED)
  add_library(vma INTERFACE)
  target_include_directories(vma INTERFACE ${VulkanMemoryAllocator_SOURCE_DIR}/include)
endif()

# stb (header-only)
CPMAddPackage(
  NAME stb
  GITHUB_REPOSITORY nothings/stb
  GIT_TAG master
  DOWNLOAD_ONLY YES
)

if(stb_ADDED)
  add_library(stb_image INTERFACE)
  target_include_directories(stb_image INTERFACE ${stb_SOURCE_DIR})
endif()

# SDL2
CPMAddPackage(
  NAME SDL
  GITHUB_REPOSITORY libsdl-org/SDL
  GIT_TAG release-2.30.3
  OPTIONS
  "SDL_SHARED OFF"
  "SDL_STATIC ON"
)

# Dear ImGui
CPMAddPackage(
  NAME imgui
  GITHUB_REPOSITORY ocornut/imgui
  VERSION 1.91.7
  DOWNLOAD_ONLY YES
)

if(imgui_ADDED)
  add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
  )
  target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
  target_link_libraries(imgui PUBLIC Vulkan::Vulkan SDL2::SDL2)
endif()

# fastgltf
CPMAddPackage(
  NAME fastgltf
  GITHUB_REPOSITORY spnda/fastgltf
  GIT_TAG v0.8.0
  OPTIONS
  "FASTGLTF_BUILD_TESTS OFF"
)

# fmt
CPMAddPackage("gh:fmtlib/fmt#11.1.3")

# ========== Project Configuration ==========
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(src)

# ========== Shader Compilation ==========
# (Keep your existing shader compilation logic here)
find_program(SLANG_COMPILER slangc HINTS
  $ENV{VULKAN_SDK}/Bin
  $ENV{VULKAN_SDK}/Bin32
  /usr/bin
  /usr/local/bin
)

file(GLOB_RECURSE SLANG_SOURCE_FILES
  "${PROJECT_SOURCE_DIR}/shaders/*.slang"
  "${PROJECT_SOURCE_DIR}/shaders/*.hlsl"
)

foreach(SLANG_FILE ${SLANG_SOURCE_FILES})
  get_filename_component(FILE_NAME ${SLANG_FILE} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${SLANG_COMPILER} -target spirv -entry main ${SLANG_FILE} -o ${SPIRV}
    DEPENDS ${SLANG_FILE}
  )
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach()

find_program(GLSL_VALIDATOR glslangValidator HINTS /usr/bin /usr/local/bin $ENV{VULKAN_SDK}/Bin/ $ENV{VULKAN_SDK}/Bin32/)

file(GLOB_RECURSE GLSL_SOURCE_FILES
  "${PROJECT_SOURCE_DIR}/shaders/*.frag"
  "${PROJECT_SOURCE_DIR}/shaders/*.vert"
  "${PROJECT_SOURCE_DIR}/shaders/*.comp"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL}
  )
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach()

add_custom_target(
  Shaders
  DEPENDS ${SPIRV_BINARY_FILES}
)