#pragma once

namespace opengl_starter
{
    struct Mesh;

    enum class RenderPassFlags
    {
        None = 0x0,
        Mesh = 0x1,
        Transparent = 0x2
    };

    struct Node
    {
        Node(const std::string name = "")
            : name(name)
        {
        }

        std::string name;

        glm::vec3 pos{};
        glm::quat rotq{};
        glm::vec3 scale{ 1.0f };

        glm::mat4 model{ 1.0f };

        Node* parent = nullptr;
        std::vector<Node*> children;

        Mesh* mesh = nullptr;
        RenderPassFlags renderPassFlags = RenderPassFlags::Mesh;
    };
}