#pragma once

#include "Common.h"

namespace opengl_starter
{
    struct Buffer;
    struct Shader;

    class DebugDraw
    {
    public:
        DebugDraw();
        ~DebugDraw();

        void NewFrame();
        void Render(const glm::mat4& view, const glm::mat4& projection);

        void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color = glm::vec4{ 1.0f }, const glm::mat4& model = glm::mat4{ 1.0f });
        void DrawBox(const glm::vec3& center, const float size, const glm::vec4& color = glm::vec4{ 1.0f }, const glm::mat4& model = glm::mat4{ 1.0f });
        void DrawCross(const glm::vec3& center, const float size, const glm::vec4& color = glm::vec4{ 1.0f }, const glm::mat4& model = glm::mat4{ 1.0f });
        void DrawArrow(const glm::vec3& start, const float length, const glm::vec4& color = glm::vec4{ 1.0f }, const glm::mat4& model = glm::mat4{ 1.0f });

    private:
        std::unique_ptr<opengl_starter::Shader> _debugDrawShader;

        GLuint _vao{ 0 };
        std::shared_ptr<Buffer> _vertexBuffer{ nullptr };
        void* _mappedBuffer{ nullptr };
        int _elements{ 0 };
    };
}
