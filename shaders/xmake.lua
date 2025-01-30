rule("shader_to_spirv")
    set_extensions(".slang", ".hlsl", ".frag", ".vert", ".comp")
    on_build_file(function(target, sourcefile, opt)
        -- Print immediately to confirm rule execution
        print("[RULE TRIGGERED] Processing: " .. sourcefile)
        
        import("lib.detect.find_program")
        local outputfile = path.join("shaders", path.basename(sourcefile) .. ".spv")

        -- Slang/HLSL compilation
        if sourcefile:endswith(".slang") or sourcefile:endswith(".hlsl") then
            local slangc = find_program("slangc", { paths = os.getenv("VULKAN_SDK") and "$(env VULKAN_SDK)/Bin" or nil })
            if not slangc then
                print("ERROR: slangc not found!")
                return
            end
            os.execv(slangc, {"-target", "spirv", "-entry", "main", sourcefile, "-o", outputfile})

        -- GLSL compilation
        else
            local glslang_validator = find_program("glslangValidator", { paths = os.getenv("VULKAN_SDK") and "$(env VULKAN_SDK)/Bin" or nil })
            if not glslang_validator then
                print("ERROR: glslangValidator not found!")
                return
            end
            os.execv(glslang_validator, {"-V", sourcefile, "-o", outputfile})
        end

        print("Generated: " .. outputfile)
        return outputfile
    end)

target("Shaders")
    set_kind("object")  -- Changed from phony
    add_rules("shader_to_spirv")
    
    -- Add shader files using pattern matching
    add_files(os.files(path.join(os.scriptdir(), "./**.slang")))
    add_files(os.files(path.join(os.scriptdir(), "./**.hlsl")))
    add_files(os.files(path.join(os.scriptdir(), "./**.frag")))
    add_files(os.files(path.join(os.scriptdir(), "./**.vert")))
    add_files(os.files(path.join(os.scriptdir(), "./**.comp")))