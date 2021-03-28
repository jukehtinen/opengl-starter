#pragma once

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

    struct Mesh
    {
        Mesh(const std::string& filename)
        {
            cgltf_options options = {};
            cgltf_data* data = nullptr;
            cgltf_result result = cgltf_parse_file(&options, filename.c_str(), &data);
            if (result != cgltf_result_success)
            {
                spdlog::error("[cgltf] Failed to load file {}", filename);
                return;
            }

            result = cgltf_load_buffers(&options, data, nullptr);

            const auto& primitive = data->meshes[0].primitives[0];

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
                    spdlog::warn("[load_mesh] Unhandled attribute {0}", attribute.name);
                    continue;
                }

                auto numComponents = cgltf_num_components(attribute.data->type);
                for (auto v = 0; v < attribute.data->count; v++)
                {
                    auto ptr = reinterpret_cast<uint8_t*>(&verts[v]) + offset;
                    cgltf_accessor_read_float(attribute.data, v, reinterpret_cast<float*>(ptr), numComponents);
                }
            }

            cgltf_free(data);

            vertexCount = static_cast<uint32_t>(verts.size());
            indexCount = static_cast<uint32_t>(indices.size());

            glCreateBuffers(1, &vbo);
            glNamedBufferData(vbo, sizeof(Vertex) * verts.size(), verts.data(), GL_STATIC_DRAW);

            glCreateBuffers(1, &ibo);
            glNamedBufferData(ibo, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

            glCreateVertexArrays(1, &vao);

            glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));
            glVertexArrayElementBuffer(vao, ibo);

            glEnableVertexArrayAttrib(vao, 0);
            glEnableVertexArrayAttrib(vao, 1);
            glEnableVertexArrayAttrib(vao, 2);

            glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
            glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
            glVertexArrayAttribFormat(vao, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));

            glVertexArrayAttribBinding(vao, 0, 0);
            glVertexArrayAttribBinding(vao, 1, 0);
            glVertexArrayAttribBinding(vao, 2, 0);
        }

        ~Mesh()
        {
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ibo);
        }

        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ibo = 0;
        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;
    };
}