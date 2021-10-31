#include "GltfLoader.h"
#include "Animation.h"

#pragma warning(push)
#pragma warning(disable : 4996) // warning C4996: '...': This function or variable may be unsafe.
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"
#pragma warning(pop)

namespace opengl_starter
{
    struct Vertex
    {
        float pos[3];
        float uv[2];
        float normal[3];
    };

    void LoadAnimations(cgltf_data* data, cgltf_node* n, opengl_starter::Node* node)
    {
        for (int a = 0; a < data->animations_count; a++)
        {
            const auto& animation = data->animations[a];

            for (int c = 0; c < animation.channels_count; c++)
            {
                const auto& channel = animation.channels[c];

                if (channel.target_node != n)
                    continue;

                spdlog::info("Animation {}", animation.name);

                const auto& sampler = channel.sampler;

                Track animTrack;

                std::string pathString;
                switch (channel.target_path)
                {
                case cgltf_animation_path_type_translation:
                    animTrack.target = Target::Translation;
                    pathString = "cgltf_animation_path_type_translation";
                    break;
                case cgltf_animation_path_type_rotation:
                    animTrack.target = Target::Rotation;
                    pathString = "cgltf_animation_path_type_rotation";
                    break;
                case cgltf_animation_path_type_scale:
                    animTrack.target = Target::Scale;
                    pathString = "cgltf_animation_path_type_scale";
                    break;
                default:
                    spdlog::warn("[GltfLoader] Unknown target_path {0}", channel.target_path);
                    break;
                }

                std::string interpolationString;
                switch (sampler->interpolation)
                {
                case cgltf_interpolation_type_linear:
                    animTrack.interpolation = Interpolation::Linear;
                    interpolationString = "cgltf_interpolation_type_linear";
                    break;
                case cgltf_interpolation_type_step:
                    animTrack.interpolation = Interpolation::Step;
                    interpolationString = "cgltf_interpolation_type_step";
                    break;
                case cgltf_interpolation_type_cubic_spline:
                    animTrack.interpolation = Interpolation::Cubic;
                    interpolationString = "cgltf_interpolation_type_cubic_spline";
                    break;
                default:
                    spdlog::warn("[GltfLoader] Unknown interpolation {0}", channel.target_path);
                    break;
                }

                auto inputFloats = cgltf_accessor_unpack_floats(sampler->input, nullptr, 0);
                auto outputFloats = cgltf_accessor_unpack_floats(sampler->output, nullptr, 0);

                animTrack.times.resize(inputFloats);
                animTrack.values.resize(outputFloats);

                cgltf_accessor_unpack_floats(sampler->input, animTrack.times.data(), inputFloats);
                cgltf_accessor_unpack_floats(sampler->output, animTrack.values.data(), outputFloats);

                spdlog::info("[GltfLoader] Channel target: {} path: {} ip: {} first_ts:{} last_ts:{} Inputs:{} Outputs:{}", channel.target_node->name, pathString, interpolationString, animTrack.times.front(), animTrack.times.back(), sampler->input->count, sampler->output->count);

                Animation animation;
                animation.tracks.push_back(std::move(animTrack));

                node->animations.push_back(std::move(animation));
            }
        }
    }

    opengl_starter::Node* GltfLoader::Load(const std::string& filename, opengl_starter::Node* parentNode, std::vector<opengl_starter::Mesh*>& outMeshes)
    {
        cgltf_options options = {};
        cgltf_data* data = nullptr;
        cgltf_result result = cgltf_parse_file(&options, filename.c_str(), &data);
        if (result != cgltf_result_success)
        {
            spdlog::error("[cgltf] cgltf_parse_file failed to load file {} ({})", filename, result);
            return nullptr;
        }

        result = cgltf_load_buffers(&options, data, nullptr);
        if (result != cgltf_result_success)
        {
            spdlog::error("[cgltf] cgltf_load_buffers failed to load file {} ({})", filename, result);
            return nullptr;
        }        

        // Have another vector to avoid name collisions.
        std::vector<opengl_starter::Mesh*> meshes;

        for (int m = 0; m < data->meshes_count; m++)
        {
            const auto& mesh = data->meshes[m];
            spdlog::info("{}, {}", mesh.name, mesh.primitives_count);
            for (int p = 0; p < mesh.primitives_count; p++)
            {
                if (p > 0)
                    spdlog::warn("[GltfLoader] {} has multiple primitives!", mesh.name);

                const auto& primitive = mesh.primitives[p];

                std::vector<uint32_t> indices(primitive.indices->count);
                for (auto v = 0; v < primitive.indices->count; v += 3)
                {
                    for (auto i = 0; i < 3; i++)
                    {
                        indices[v + i] = static_cast<uint32_t>(cgltf_accessor_read_index(primitive.indices, v + i));
                    }
                }

                std::vector<Vertex> verts(primitive.attributes[0].data->count);
                for (auto i = 0; i < primitive.attributes_count; i++)
                {
                    const auto& attribute = primitive.attributes[i];

                    cgltf_size offset = 0;
                    switch (attribute.type)
                    {
                    case cgltf_attribute_type::cgltf_attribute_type_position:
                        offset = offsetof(Vertex, pos);
                        break;
                    case cgltf_attribute_type::cgltf_attribute_type_texcoord:
                        offset = offsetof(Vertex, uv);
                        break;
                    case cgltf_attribute_type::cgltf_attribute_type_normal:
                        offset = offsetof(Vertex, normal);
                        break;
                    default:
                        spdlog::warn("[GltfLoader] Unhandled attribute {0}", attribute.name);
                        continue;
                    }

                    auto numComponents = cgltf_num_components(attribute.data->type);
                    for (auto v = 0; v < attribute.data->count; v++)
                    {
                        auto ptr = reinterpret_cast<uint8_t*>(&verts[v]) + offset;
                        cgltf_accessor_read_float(attribute.data, v, reinterpret_cast<float*>(ptr), numComponents);
                    }
                }

                auto primitiveMesh = new Mesh;
                primitiveMesh->name = p == 0 ? mesh.name : fmt::format("{}.{}", mesh.name, p);
                primitiveMesh->vertexCount = static_cast<uint32_t>(verts.size());
                primitiveMesh->indexCount = static_cast<uint32_t>(indices.size());
                primitiveMesh->vertexBuffer = std::make_shared<Buffer>(sizeof(Vertex) * verts.size(), verts.data());
                primitiveMesh->indexBuffer = std::make_shared<Buffer>(sizeof(uint32_t) * indices.size(), indices.data());

                glCreateVertexArrays(1, &primitiveMesh->vao);

                glVertexArrayVertexBuffer(primitiveMesh->vao, 0, primitiveMesh->vertexBuffer->buffer, 0, sizeof(Vertex));
                glVertexArrayElementBuffer(primitiveMesh->vao, primitiveMesh->indexBuffer->buffer);

                glEnableVertexArrayAttrib(primitiveMesh->vao, 0);
                glEnableVertexArrayAttrib(primitiveMesh->vao, 1);
                glEnableVertexArrayAttrib(primitiveMesh->vao, 2);

                glVertexArrayAttribFormat(primitiveMesh->vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
                glVertexArrayAttribFormat(primitiveMesh->vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
                glVertexArrayAttribFormat(primitiveMesh->vao, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));

                glVertexArrayAttribBinding(primitiveMesh->vao, 0, 0);
                glVertexArrayAttribBinding(primitiveMesh->vao, 1, 0);
                glVertexArrayAttribBinding(primitiveMesh->vao, 2, 0);

                meshes.push_back(primitiveMesh);
            }
        }

        std::copy(meshes.begin(), meshes.end(), std::back_inserter(outMeshes));

        opengl_starter::Node* sceneRoot = nullptr;
        if (parentNode != nullptr)
        {
            auto createNode = [&meshes](cgltf_data* data, opengl_starter::Node* parent, cgltf_node* n, auto& createNodeRef) -> void {
                auto node = new opengl_starter::Node{ fmt::format("{}", n->name) };
                node->pos = glm::make_vec3(n->translation);
                node->scale = glm::make_vec3(n->scale);
                node->rotq = glm::make_quat(n->rotation);

                LoadAnimations(data, n, node);

                if (n->mesh != nullptr)
                {
                    auto it = std::find_if(meshes.begin(), meshes.end(), [n](const Mesh* mesh) { return mesh->name == std::string(n->mesh->name); });
                    node->mesh = *it;
                }

                parent->children.push_back(node);

                for (int m = 0; m < n->children_count; m++)
                {
                    createNodeRef(data, node, n->children[m], createNodeRef);
                }
            };

            sceneRoot = new opengl_starter::Node{ fmt::format("{}", data->scenes[0].name) };

            for (int m = 0; m < data->scenes[0].nodes_count; m++)
            {
                const auto n = data->scenes[0].nodes[m];
                createNode(data, sceneRoot, n, createNode);
            }

            parentNode->children.push_back(sceneRoot);
        }

        cgltf_free(data);

        return sceneRoot;
    }
}