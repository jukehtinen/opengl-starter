#pragma once

#include "Common.h"
#include "imgui.h"
#include "lodepng.h"

#include <random>

namespace opengl_starter
{
    struct SSAO
    {
        SSAO()
        {
            GenerateKernel();
        }

        void OnUI()
        {
            ImGui::Begin("SSAO");
            ImGui::Checkbox("enableAo", &enableAo);
            ImGui::Checkbox("visualizeAo", &visualizeAo);

            ImGui::DragFloat("radius", &radius, 0.001f, 0.0f, 6.0f, "%.5f");
            ImGui::DragFloat("strength", &strength, 0.001f, 0.0f, 128.0f, "%.5f");
            ImGui::DragFloat("bias", &bias, 0.001f, 0.0f, 1.0f, "%.5f");
            if (ImGui::DragInt("kernel", &kernelSize, 1, 8, 512))
            {
                GenerateKernel();
            }
            ImGui::End();
        }

        void GenerateKernel()
        {
            ssaoKernel.resize(kernelSize);

            std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
            std::default_random_engine generator;
            for (int i = 0; i < kernelSize; ++i)
            {
                glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
                sample = glm::normalize(sample);
                sample *= randomFloats(generator);

                float scale = float(i) / float(kernelSize);

                // scale samples s.t. they're more aligned to center of kernel
                scale = glm::mix(0.1f, 1.0f, scale * scale);
                sample *= scale;
                ssaoKernel[i] = sample;
            }
        }

        std::vector<glm::vec3> ssaoKernel;

        float radius = 0.5f;
        float strength = 1.0f;
        float bias = 0.025f;

        int kernelSize = 128;

        bool enableAo = true;
        bool visualizeAo = false;
    };
}