#pragma once

#include "Common.h"

namespace opengl_starter
{
    struct Shader;
    struct Mesh;
    struct Texture;

    class Grass
    {
    public:
        Grass(opengl_starter::Mesh* grassMesh);
        ~Grass();

        void Update(float delta, float totalTime);
        void Render(const glm::mat4& projection, const glm::mat4& view);

        void OnUI();

    private:
        GLuint _transformsBuffer{ 0 };
        std::unique_ptr<opengl_starter::Shader> _grassShader;
        opengl_starter::Mesh* _grassMesh{ nullptr };
        std::unique_ptr<opengl_starter::Texture> _noiseTexture{ nullptr };
        float _totalTime{ 0.0f };
    };
}