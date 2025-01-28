add_rules("mode.debug", "mode.release")

-- Set output directories
set_targetdir("bin")
set_objectdir("build/obj")

-- Add required packages
add_requires("vulkansdk")
add_requires("imgui", {configs = {sdl2 = true, vulkan = true}})  -- ImGui library
add_requires("fastgltf")  -- FastGLTF library
add_requires("fmt")  -- fmt library
add_requires("glm")  -- GLM library (header-only)
add_requires("stb")  -- stb_image library (header-only)

includes("shaders")

-- SDL2 Target
target("sdl2")
    set_kind("static") -- Or "phony"
    add_includedirs("third_party/SDL/include") -- Include SDL2 headers
    add_linkdirs("third_party/SDL")        -- Link SDL2 libraries
    if is_plat("windows") then
        add_links("SDL2", "SDL2main")           -- Add SDL2 libraries on Windows
    elseif is_plat("linux") then
        add_links("SDL2")                       -- Add SDL2 libraries on Linux
    end

-- VkBootstrap Target
target("vkbootstrap")
    set_kind("static")
    add_packages("vulkansdk")
    add_files("third_party/vkbootstrap/VkBootstrap.cpp")
    add_includedirs("third_party/vkbootstrap", {public = true})
    if is_plat("linux", "macosx") then
        add_syslinks("dl")
    end
    set_languages("cxx20")

-- VMA Target
target("vma")
    set_kind("headeronly")
    add_packages("vulkansdk")
    add_includedirs("third_party/vma", {public = true})

-- Main target
target("VoltexEngine")
    set_kind("binary")
    set_languages("cxx20")
    add_defines("GLM_FORCE_DEPTH_ZERO_TO_ONE", "GLM_ENABLE_EXPERIMENTAL")
    add_deps("sdl2", "vkbootstrap", "vma")
    add_packages("vulkan", "fastgltf", "fmt", "imgui", "glm", "stb")
    add_files("src/**.cpp")
    add_includedirs(os.dirs(path.join(os.scriptdir(), "src/**")))
