#include "stb_image.h"
#include <vk_loader.h>

#include "vk_engine.h"
#include "vk_initializers.h"
#include "vk_types.h"
#include <glm/gtx/quaternion.hpp>

#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>

std::optional<std::vector<std::shared_ptr<MeshAsset>>> loadGltfMeshes(VulkanEngine* engine, std::string path) {
#ifndef PROJECT_ROOT
    fmt::println("PROJECT_ROOT must be defined in src/CMakeLists.txt:\ntarget_compile_definitions(engine PRIVATE PROJECT_ROOT=\"${CMAKE_SOURCE_DIR}\")");
    return {};
#else
    std::filesystem::path filePath = std::string(PROJECT_ROOT) + "/" + path;
#endif
    fmt::println("Loading GLTF: {}", filePath.string());

    auto gltfFile = fastgltf::MappedGltfFile::FromPath(filePath);
    if (!bool(gltfFile)) {
        fmt::println("Failed to open glTF file: {}", fastgltf::getErrorMessage(gltfFile.error()));
        return {};
    }

    fastgltf::Asset gltf;
    fastgltf::Parser parser{};

    auto asset = parser.loadGltfBinary(gltfFile.get(), filePath.parent_path(), fastgltf::Options::LoadExternalBuffers);
    if (asset.error() != fastgltf::Error::None) {
        fmt::println("Failed to load glTF: {}", fastgltf::getErrorMessage(asset.error()));
        return {};
    }

    gltf = std::move(asset.get());

    std::vector<std::shared_ptr<MeshAsset>> meshes;

    std::vector<uint32_t> indices;
    std::vector<Vertex> vertices;
    for (fastgltf::Mesh& mesh : gltf.meshes) {
        MeshAsset newmesh;
        newmesh.name = mesh.name;

        indices.clear();
        vertices.clear();

        for (auto&& p : mesh.primitives) {
            GeoSurface newSurface;
            newSurface.startIndex = static_cast<uint32_t>(indices.size());
            newSurface.count = static_cast<uint32_t>(gltf.accessors[p.indicesAccessor.value()].count);

            size_t initial_vtx = vertices.size();

            // Load indices
            {
                fastgltf::Accessor& indexaccessor = gltf.accessors[p.indicesAccessor.value()];
                indices.reserve(indices.size() + indexaccessor.count);

                fastgltf::iterateAccessor<std::uint32_t>(gltf, indexaccessor,
                    [&](std::uint32_t idx) {
                        indices.push_back(static_cast<uint32_t>(idx + initial_vtx));
                    });
            }

            // Load vertex positions
            {
                auto posAttr = p.findAttribute("POSITION");
                if (posAttr != p.attributes.end()) {
                    fastgltf::Accessor& posAccessor = gltf.accessors[posAttr->accessorIndex];
                    vertices.resize(vertices.size() + posAccessor.count);

                    fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, posAccessor,
                        [&](glm::vec3 v, size_t index) {
                            Vertex newvtx;
                            newvtx.position = v;
                            newvtx.normal = { 1, 0, 0 };
                            newvtx.color = glm::vec4{ 1.f };
                            newvtx.uv_x = 0;
                            newvtx.uv_y = 0;
                            vertices[initial_vtx + index] = newvtx;
                        });
                }
            }

            // Load vertex normals
            auto normals = p.findAttribute("NORMAL");
            if (normals != p.attributes.end()) {
                fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, gltf.accessors[normals->accessorIndex],
                    [&](glm::vec3 v, size_t index) {
                        vertices[initial_vtx + index].normal = v;
                    });
            }

            // Load UVs
            auto uv = p.findAttribute("TEXCOORD_0");
            if (uv != p.attributes.end()) {
                fastgltf::iterateAccessorWithIndex<glm::vec2>(gltf, gltf.accessors[uv->accessorIndex],
                    [&](glm::vec2 v, size_t index) {
                        vertices[initial_vtx + index].uv_x = v.x;
                        vertices[initial_vtx + index].uv_y = v.y;
                    });
            }

            // Load vertex colors
            auto colors = p.findAttribute("COLOR_0");
            if (colors != p.attributes.end()) {
                fastgltf::iterateAccessorWithIndex<glm::vec4>(gltf, gltf.accessors[colors->accessorIndex],
                    [&](glm::vec4 v, size_t index) {
                        vertices[initial_vtx + index].color = v;
                    });
            }
            newmesh.surfaces.push_back(newSurface);
        }

        constexpr bool OverrideColors = true;
        if (OverrideColors) {
            for (Vertex& vtx : vertices) {
                vtx.color = glm::vec4(vtx.normal, 1.f);
            }
        }
        newmesh.meshBuffers = engine->uploadMesh(indices, vertices);

        meshes.emplace_back(std::make_shared<MeshAsset>(std::move(newmesh)));
    }

    return meshes;
}