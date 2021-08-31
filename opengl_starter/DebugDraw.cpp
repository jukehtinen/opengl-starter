#include "DebugDraw.h"
#include "Buffer.h"
#include "Shader.h"

namespace opengl_starter
{
    constexpr int MaxVerts = 10000;

    struct Vertex
    {
        glm::vec3 pos;
        glm::vec4 color;
    };

    DebugDraw::DebugDraw()
    {
        _debugDrawShader = std::make_unique<opengl_starter::Shader>("assets/debug-draw.vert", "assets/debug-draw.frag");

        _vertexBuffer = std::make_shared<Buffer>(MaxVerts * sizeof(Vertex));

        glCreateVertexArrays(1, &_vao);

        glVertexArrayVertexBuffer(_vao, 0, _vertexBuffer->buffer, 0, sizeof(Vertex));

        glEnableVertexArrayAttrib(_vao, 0);
        glEnableVertexArrayAttrib(_vao, 1);

        glVertexArrayAttribFormat(_vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
        glVertexArrayAttribFormat(_vao, 1, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));

        glVertexArrayAttribBinding(_vao, 0, 0);
        glVertexArrayAttribBinding(_vao, 1, 0);

        _mappedBuffer = glMapNamedBufferRange(_vertexBuffer->buffer, 0, static_cast<GLintptr>(MaxVerts) * sizeof(Vertex),
            GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    }

    DebugDraw::~DebugDraw()
    {
        glUnmapNamedBuffer(_vertexBuffer->buffer);
        glDeleteVertexArrays(1, &_vao);
    }

    void DebugDraw::NewFrame()
    {
        _elements = 0;
    }

    void DebugDraw::Render(const glm::mat4& view, const glm::mat4& projection)
    {
        _debugDrawShader->Bind();
        _debugDrawShader->SetMat4("vp", projection * view);

        glLineWidth(4.0f);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindVertexArray(_vao);
        glDrawArrays(GL_LINES, 0, _elements);
        glBindVertexArray(0);

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
    }

    void DebugDraw::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, const glm::mat4& model)
    {
        // Todo: buffer could be resized on-demand.
        if (_elements + 2 >= MaxVerts)
            return;

        auto buffer = reinterpret_cast<Vertex*>(_mappedBuffer);

        buffer[_elements].color = color;
        buffer[_elements++].pos = model * glm::vec4{ p0, 1.0f };
        buffer[_elements].color = color;
        buffer[_elements++].pos = model * glm::vec4{ p1, 1.0f };
    }

    void DebugDraw::DrawBox(const glm::vec3& center, const float size, const glm::vec4& color, const glm::mat4& model)
    {
        const float half = size / 2.0f;

        glm::vec3 corner1 = center - glm::vec3{ half };
        DrawLine(corner1, corner1 + glm::vec3{ size, 0.0f, 0.0f }, color, model);
        DrawLine(corner1, corner1 + glm::vec3{ 0.0f, size, 0.0f }, color, model);
        DrawLine(corner1, corner1 + glm::vec3{ 0.0f, 0.0f, size }, color, model);

        glm::vec3 corner2 = center + glm::vec3{ half };
        DrawLine(corner2, corner2 - glm::vec3{ size, 0.0f, 0.0f }, color, model);
        DrawLine(corner2, corner2 - glm::vec3{ 0.0f, size, 0.0f }, color, model);
        DrawLine(corner2, corner2 - glm::vec3{ 0.0f, 0.0f, size }, color, model);
    }

    void DebugDraw::DrawCross(const glm::vec3& center, const float size, const glm::vec4& color, const glm::mat4& model)
    {
        const float half = size / 2.0f;
        DrawLine({ center.x - half, center.y, center.z }, { center.x + half, center.y, center.z }, color, model);
        DrawLine({ center.x, center.y - half, center.z }, { center.x, center.y + half, center.z }, color, model);
        DrawLine({ center.x, center.y, center.z - half }, { center.x, center.y, center.z + half }, color, model);
    }

    void DebugDraw::DrawArrow(const glm::vec3& start, const float length, const glm::vec4& color, const glm::mat4& model)
    {
        const glm::vec3 end = start + (glm::vec3{ 0.0f, 1.0f, 0.0f } * length);
        DrawLine(start, end, color, model);
        DrawCross(end, 0.3f, color, model);
    }
}