#include "Grass.h"

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Utils.h"

#include "imgui.h"

#include <vector>

namespace opengl_starter
{
    constexpr int MaxGrass = 1000;

    Grass::Grass(opengl_starter::Mesh* grassMesh)
        : _grassMesh(grassMesh)
    {
        _grassShader = std::make_unique<opengl_starter::Shader>("assets/grass.vert", "assets/grass.frag");
        _noiseTexture = std::make_unique<opengl_starter::Texture>("assets/noise.png");

        std::vector<glm::mat4> transforms(MaxGrass);

        float areaHalfLength = 7.0f;
        float padding = 1.0f;
        float noise = 0.4f;
        int index = 0;
        float y = -areaHalfLength;
        while (y < areaHalfLength * 2.0f)
        {
            float x = -areaHalfLength;
            while (x < areaHalfLength * 2.0f)
            {
                if (transforms.size() <= index)
                    break;

                glm::vec3 offset{ Utils::Rng::RandomFloat(-noise, noise), 0.0f, Utils::Rng::RandomFloat(-noise, noise) };

                transforms[index++] = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ x, 0.0f, y } + offset) *
                                      glm::rotate(glm::mat4{ 1.0f }, glm::radians(360.0f), { 0.0f, 1.0f, 0.0f });

                x += padding;
            }
            y += padding;
        }

        glCreateBuffers(1, &_transformsBuffer);
        glNamedBufferData(_transformsBuffer, sizeof(glm::mat4) * MaxGrass, transforms.data(), GL_STATIC_DRAW);
    }

    Grass::~Grass()
    {
        glDeleteBuffers(1, &_transformsBuffer);
    }

    void Grass::Update(float delta, float totalTime)
    {
        _totalTime = totalTime;
    }

    void Grass::Render(const glm::mat4& projection, const glm::mat4& view)
    {
        glDisable(GL_CULL_FACE);
        glBindProgramPipeline(_grassShader->pipeline);
        _grassShader->SetMat4("vp", projection * view);
        _grassShader->SetMat4("view", view);
        _grassShader->SetMat4("m", glm::translate(glm::mat4{ 1.0f }, glm::vec3{ 35.0f, -0.1f, 0.0f }));
        _grassShader->SetFloatVs("time", _totalTime);

        auto block_index = glGetProgramResourceIndex(_grassShader->vertProg, GL_SHADER_STORAGE_BLOCK, "modelMatrices");
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, _transformsBuffer, 0, MaxGrass * sizeof(glm::mat4));
        glShaderStorageBlockBinding(_grassShader->vertProg, block_index, 0);

        glBindTextureUnit(2, _noiseTexture->textureName);

        glBindVertexArray(_grassMesh->vao);

        glDrawElementsInstanced(GL_TRIANGLES, _grassMesh->indexCount, GL_UNSIGNED_INT, nullptr, MaxGrass);

        glBindVertexArray(0);
        glEnable(GL_CULL_FACE);
    }

    void Grass::OnUI()
    {
        ImGui::Begin("Shader");

        for (const auto& uniform : _grassShader->GetUniforms())
        {
            glm::vec4 vec4 = uniform.value;

            switch (uniform.type)
            {
            case GL_FLOAT:
                if (ImGui::DragFloat(uniform.name.c_str(), glm::value_ptr(vec4), 0.1f, 0.0f, 20.0f))
                    _grassShader->SetFloatVs(uniform.name, vec4.x);
                break;
            case GL_FLOAT_VEC2:
                if (ImGui::DragFloat2(uniform.name.c_str(), glm::value_ptr(vec4), 0.1f, 0.0f, 20.0f))
                    _grassShader->SetVec2(uniform.name, { vec4.x, vec4.y });
                break;
            case GL_FLOAT_VEC3:
                if (ImGui::DragFloat3(uniform.name.c_str(), glm::value_ptr(vec4), 0.1f, 0.0f, 20.0f))
                    _grassShader->SetVec3(uniform.name, { vec4.x, vec4.y, vec4.z });
                break;
            case GL_FLOAT_VEC4:
                if (ImGui::DragFloat4(uniform.name.c_str(), glm::value_ptr(vec4), 0.1f, 0.0f, 20.0f))
                    _grassShader->SetVec4(uniform.name, vec4);
                break;
            default:
                break;
            }
        }

        ImGui::End();
    }
}