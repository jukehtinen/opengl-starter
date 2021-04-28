#pragma once

#include "Common.h"
#include "Utils.h"
#include "imgui.h"

namespace opengl_starter
{
    struct Decal
    {
        glm::vec3 pos{};
        glm::vec3 rot{};
        glm::vec3 scale = glm::vec3{ 1.0f };
        glm::vec4 color{};
    };

    class Decals
    {
    public:
        void OnDecalUI()
        {
            ImGui::Begin("Decal Renderer");

            if (ImGui::Button("Add"))
            {
                glm::vec3 hsv{ Utils::Rng::RandomFloat() * 360.0f, 1.0, 1.0f };

                Decal decal;
                decal.pos.x = Utils::Rng::RandomFloat() * 20.0f - 10.0f;
                decal.pos.z = Utils::Rng::RandomFloat() * 20.0f - 10.0f;
                decal.rot.y = Utils::Rng::RandomFloat() * 360.0f;
                decal.color = glm::vec4{ glm::rgbColor(hsv), 1.0f };

                decals.push_back(decal);
            }

            if (ImGui::BeginListBox("Decals"))
            {
                for (int n = 0; n < decals.size(); n++)
                {
                    const bool selected = (currentIndex == n);
                    if (ImGui::Selectable(fmt::format("Decal {}", n).c_str(), selected))
                        currentIndex = n;

                    if (selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndListBox();
            }

            if (currentIndex != -1)
            {
                ImGui::DragFloat3("Pos", glm::value_ptr(decals[currentIndex].pos), 0.1f, -1000.0f, 1000.0f);
                ImGui::DragFloat3("Rot", glm::value_ptr(decals[currentIndex].rot), 0.1f, -360.0f, 360.0f);
                ImGui::DragFloat3("Scl", glm::value_ptr(decals[currentIndex].scale), 0.1f, 0.1f, 10.0f);
            }

            ImGui::End();
        }

        std::vector<Decal> decals;

        int currentIndex = -1;
    };
}
