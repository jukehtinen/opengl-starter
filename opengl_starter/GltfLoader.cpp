#include "GltfLoader.h"

#pragma warning(push)
#pragma warning(disable : 4996) // warning C4996: '...': This function or variable may be unsafe.
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"
#pragma warning(pop)

using namespace opengl_starter;

struct Vertex
{
    float pos[3];
    float uv[2];
    float normal[3];
};

void GltfLoader::Load(const std::string& filename, opengl_starter::Node* parentNode, std::vector<opengl_starter::Mesh*>& outMeshes)
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

    if (parentNode != nullptr)
    {
        auto createNode = [&meshes](opengl_starter::Node* parent, cgltf_node* n, auto& createNodeRef) -> void {
            auto node = new opengl_starter::Node{ fmt::format("{}", n->name) };
            node->pos = glm::make_vec3(n->translation);
            node->scale = glm::make_vec3(n->scale);
            node->rotq = glm::make_quat(n->rotation);

            if (n->mesh != nullptr)
            {
                auto it = std::find_if(meshes.begin(), meshes.end(), [n](const Mesh* mesh) { return mesh->name == std::string(n->mesh->name); });
                node->mesh = *it;
            }

            parent->children.push_back(node);

            for (int m = 0; m < n->children_count; m++)
            {
                createNodeRef(node, n->children[m], createNodeRef);
            }
        };

        auto sceneRoot = new opengl_starter::Node{ fmt::format("{}", data->scenes[0].name) };

        for (int m = 0; m < data->scenes[0].nodes_count; m++)
        {
            const auto n = data->scenes[0].nodes[m];
            createNode(sceneRoot, n, createNode);
        }

        parentNode->children.push_back(sceneRoot);
    }

    cgltf_free(data);
}
