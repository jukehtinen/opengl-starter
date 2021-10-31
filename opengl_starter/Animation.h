#pragma once

#include "Common.h"

#include <vector>

namespace opengl_starter
{
    struct Node;

    enum class Interpolation
    {
        Linear,
        Step,
        Cubic
    };

    enum class Target
    {
        None,
        Translation,
        Rotation,
        Scale
    };

    // https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/gltfTutorial_007_Animations.md
    struct Track
    {
        std::vector<float> times;
        std::vector<float> values;

        Interpolation interpolation{ Interpolation::Linear };
        Target target{ Target::None };

        void Animate(float t, opengl_starter::Node* node);

        const glm::vec3 Cubic(int previousTimeIndex, int nextTimeIndex, float deltaTime, float interpolationValue) const;
        const glm::quat CubicQ(int previousTimeIndex, int nextTimeIndex, float deltaTime, float interpolationValue) const;
        const glm::vec3 LinearV3(int previousTimeIndex, int nextTimeIndex, float interpolationValue) const;
        const glm::quat LinearQ(int previousTimeIndex, int nextTimeIndex, float interpolationValue) const;
    };

    struct Animation
    {
        void Animate(float t, opengl_starter::Node* node);

        std::vector<Track> tracks;
    };
}