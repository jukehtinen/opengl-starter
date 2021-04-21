#pragma once

#include "Common.h"

namespace opengl_starter
{
    struct Buffer;
    struct Shader;
    struct Texture;

    class Terrain
    {
    public:
        Terrain(bool tesselated, opengl_starter::Shader* shader,
            opengl_starter::Texture* texHeight,
            opengl_starter::Texture* texColor,
            opengl_starter::Texture* texColor2);
        ~Terrain();

        void Render(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& eye);

    private:
        opengl_starter::Shader* _shader = nullptr;
        opengl_starter::Texture* _texHeight = nullptr;
        opengl_starter::Texture* _texColor = nullptr;
        opengl_starter::Texture* _texColor2 = nullptr;

        bool _tesselated = false;

        GLuint _vao = 0;
        std::shared_ptr<Buffer> _vertexBuffer = nullptr;
        std::shared_ptr<Buffer> _indexBuffer = nullptr;
        GLuint _elements = 0;
    };
}