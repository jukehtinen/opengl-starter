#pragma once

#include "imgui.h"

namespace opengl_starter
{
    class Bloom
    {
    public:
        void OnUI()
        {
            ImGui::Begin("Bloom");
            ImGui::DragFloat("threshold", &threshold, 0.01f, 0.0f, 2.0f);
            ImGui::Checkbox("enableBloom", &enable);
            ImGui::Checkbox("visualizeBloom", &visualize);
            ImGui::End();
        }

        float threshold{ 0.6f };
        bool enable{ true };
        bool visualize{ false };
    };
}
