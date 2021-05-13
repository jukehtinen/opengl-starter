#pragma once

namespace opengl_starter
{
    struct Buffer;

    struct Mesh
    {
        Mesh()
        {
        }

        ~Mesh()
        {
            glDeleteVertexArrays(1, &vao);
        }

        std::string name;

        GLuint vao = 0;
        std::shared_ptr<Buffer> vertexBuffer = nullptr;
        std::shared_ptr<Buffer> indexBuffer = nullptr;
        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;
    };
}