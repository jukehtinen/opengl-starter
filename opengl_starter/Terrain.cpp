#include "Terrain.h"
#include "Buffer.h"
#include "Shader.h"
#include "Texture.h"

#include <vector>

using namespace opengl_starter;

struct Vertex
{
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 normal;
};

Terrain::Terrain(bool tesselated, opengl_starter::Shader* shader,
    opengl_starter::Texture* texHeight,
    opengl_starter::Texture* texColor,
    opengl_starter::Texture* texColor2)
    : _tesselated(tesselated), _shader(shader), _texHeight(texHeight), _texColor(texColor), _texColor2(texColor2)
{
    const glm::vec3 normal{ 0.0f, 1.0f, 0.0f };
    const glm::vec4 floorColor{ 0.3f, 0.3f, 0.3f, 1.0f };
    const uint32_t quadIndices[] = { 0, 1, 2, 1, 3, 2 };

    int resolution = 50;

    int numVerts = resolution * resolution;
    int numFaces = (resolution - 1) * (resolution - 1);
    int numIndices = numFaces * 6;

    std::vector<Vertex> vertices(numVerts);
    std::vector<uint32_t> indices(numIndices);

    for (int i = 0; i < resolution * resolution; i++)
    {
        float tx = (i % resolution);
        float ty = (i / resolution);

        float x = (float)tx / (float)(resolution - 1);
        float y = (float)ty / (float)(resolution - 1);
        vertices[i] = { glm::vec3{ x, 0.0f, y }, glm::vec2{ x, y }, normal };
    }

    int offset = 0;
    for (int i = 0; i < indices.size();)
    {
        unsigned int cornerIndex = i / 6 + offset;

        if ((cornerIndex + 1) % resolution == 0)
        {
            offset++;
            cornerIndex++;
        }

        indices[i++] = (unsigned int)cornerIndex;
        indices[i++] = (unsigned int)cornerIndex + resolution;
        indices[i++] = (unsigned int)cornerIndex + resolution + 1;
        indices[i++] = (unsigned int)cornerIndex;
        indices[i++] = (unsigned int)cornerIndex + resolution + 1;
        indices[i++] = (unsigned int)cornerIndex + 1;
    }

    _vertexBuffer = std::make_shared<Buffer>(numVerts * sizeof(Vertex), vertices.data());
    _indexBuffer = std::make_shared<Buffer>(numIndices * sizeof(uint32_t), indices.data());
    _elements = indices.size();

    glCreateVertexArrays(1, &_vao);

    glVertexArrayVertexBuffer(_vao, 0, _vertexBuffer->buffer, 0, sizeof(Vertex));
    glVertexArrayElementBuffer(_vao, _indexBuffer->buffer);

    glEnableVertexArrayAttrib(_vao, 0);
    glEnableVertexArrayAttrib(_vao, 1);
    glEnableVertexArrayAttrib(_vao, 2);

    glVertexArrayAttribFormat(_vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
    glVertexArrayAttribFormat(_vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
    glVertexArrayAttribFormat(_vao, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));

    glVertexArrayAttribBinding(_vao, 0, 0);
    glVertexArrayAttribBinding(_vao, 1, 0);
    glVertexArrayAttribBinding(_vao, 2, 0);
}

Terrain::~Terrain()
{
    glDeleteVertexArrays(1, &_vao);
}

void Terrain::Render(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& eye)
{
    auto scale = 50.0f;
    auto model = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ -(scale / 2.0f), 0.0f, -(scale / 2.0f) }) * glm::scale(glm::mat4{ 1.0f }, glm::vec3{ scale, 1.0f, scale });

    if (_tesselated)
    {
        glBindProgramPipeline(_shader->pipeline);
        glProgramUniformMatrix4fv(_shader->vertProg, glGetUniformLocation(_shader->vertProg, "vp"), 1, GL_FALSE, glm::value_ptr(projection * view));
        glProgramUniformMatrix4fv(_shader->vertProg, glGetUniformLocation(_shader->vertProg, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glProgramUniform1i(_shader->vertProg, glGetUniformLocation(_shader->vertProg, "texHeight"), 0);
        glProgramUniform1i(_shader->vertProg, glGetUniformLocation(_shader->vertProg, "texHeight"), 0);
        glProgramUniform1f(_shader->vertProg, glGetUniformLocation(_shader->vertProg, "displacement"), 1.0f);
        glProgramUniform3fv(_shader->tescProg, glGetUniformLocation(_shader->tescProg, "eye"), 1, glm::value_ptr(eye));
        glProgramUniform1f(_shader->teseProg, glGetUniformLocation(_shader->teseProg, "displacement"), 1.0f);
        glProgramUniformMatrix4fv(_shader->teseProg, glGetUniformLocation(_shader->teseProg, "vp"), 1, GL_FALSE, glm::value_ptr(projection * view));
        glProgramUniform1i(_shader->teseProg, glGetUniformLocation(_shader->teseProg, "texHeight"), 0);
        glProgramUniform1i(_shader->fragProg, glGetUniformLocation(_shader->fragProg, "texColor"), 1);
        glProgramUniform1i(_shader->fragProg, glGetUniformLocation(_shader->fragProg, "texColor2"), 2);
        glProgramUniform1f(_shader->fragProg, glGetUniformLocation(_shader->fragProg, "white"), 0.0f);
        glBindTextureUnit(0, _texHeight->textureName);
        glBindTextureUnit(1, _texColor->textureName);
        glBindTextureUnit(2, _texColor2->textureName);

        glBindVertexArray(_vao);
        glPatchParameteri(GL_PATCH_VERTICES, 3);
        glDrawElements(GL_PATCHES, _elements, GL_UNSIGNED_INT, nullptr);

        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPolygonOffset(-1.0f, 1.0f);
        glProgramUniform1f(_shader->fragProg, glGetUniformLocation(_shader->fragProg, "white"), 1.0f);
        glDrawElements(GL_PATCHES, _elements, GL_UNSIGNED_INT, nullptr);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glPolygonOffset(0.0f, 0.0f);
        glDisable(GL_POLYGON_OFFSET_LINE);

        glBindVertexArray(0);
    }
    else
    {
        glBindProgramPipeline(_shader->pipeline);
        glProgramUniformMatrix4fv(_shader->vertProg, glGetUniformLocation(_shader->vertProg, "vp"), 1, GL_FALSE, glm::value_ptr(projection * view));
        glProgramUniformMatrix4fv(_shader->vertProg, glGetUniformLocation(_shader->vertProg, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glProgramUniform1f(_shader->vertProg, glGetUniformLocation(_shader->vertProg, "displacement"), 1.0f);
        glProgramUniform1i(_shader->vertProg, glGetUniformLocation(_shader->vertProg, "texHeight"), 0);
        glProgramUniform1i(_shader->fragProg, glGetUniformLocation(_shader->fragProg, "texHeight"), 0);
        glProgramUniform1i(_shader->fragProg, glGetUniformLocation(_shader->fragProg, "texColor"), 1);
        glProgramUniform1i(_shader->fragProg, glGetUniformLocation(_shader->fragProg, "texColor2"), 2);
        glProgramUniform1f(_shader->fragProg, glGetUniformLocation(_shader->fragProg, "white"), 0.0f);

        glBindTextureUnit(0, _texHeight->textureName);
        glBindTextureUnit(1, _texColor->textureName);
        glBindTextureUnit(2, _texColor2->textureName);

        glBindVertexArray(_vao);
        glDrawElements(GL_TRIANGLES, _elements, GL_UNSIGNED_INT, nullptr);

        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPolygonOffset(-1.0f, 1.0f);
        glProgramUniform1f(_shader->fragProg, glGetUniformLocation(_shader->fragProg, "white"), 1.0f);
        glDrawElements(GL_TRIANGLES, _elements, GL_UNSIGNED_INT, nullptr);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glPolygonOffset(0.0f, 0.0f);
        glDisable(GL_POLYGON_OFFSET_LINE);

        glBindVertexArray(0);
    }
}