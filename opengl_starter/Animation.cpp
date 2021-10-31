#include "Animation.h"
#include "Node.h"

#include <glm/gtx/spline.hpp>

namespace opengl_starter
{
    void Track::Animate(float t, opengl_starter::Node* node)
    {
        t = fmodf(t, times[times.size() - 1]);
        t = glm::clamp(t, times.front(), times.back());

        int previousTimeIndex = 0;
        int nextTimeIndex = 0;

        for (int i = 0; i < times.size(); i++)
        {
            if (times[i] > t)
            {
                nextTimeIndex = i;
                break;
            }

            previousTimeIndex = i;
        }

        const float nextTime = times[nextTimeIndex];
        const float previousTime = times[previousTimeIndex];

        float interpolationValue = (t - previousTime) / (nextTime - previousTime);

        switch (target)
        {
        case Target::Rotation:
            switch (interpolation)
            {
            case Interpolation::Cubic: {
                node->rotq = CubicQ(previousTimeIndex, nextTimeIndex, nextTime - previousTime, interpolationValue);
                break;
            }
            case Interpolation::Linear: {
                node->rotq = LinearQ(previousTimeIndex, nextTimeIndex, interpolationValue);
                break;
            }
            default:
                break;
            }
            break;
        case Target::Translation:
            switch (interpolation)
            {
            case Interpolation::Cubic: {
                node->pos = Cubic(previousTimeIndex, nextTimeIndex, nextTime - previousTime, interpolationValue);
                break;
            }
            case Interpolation::Linear: {
                node->pos = LinearV3(previousTimeIndex, nextTimeIndex, interpolationValue);
                break;
            }
            default:
                break;
            }
            break;
        case Target::Scale:
            switch (interpolation)
            {
            case Interpolation::Cubic: {
                node->scale = Cubic(previousTimeIndex, nextTimeIndex, nextTime - previousTime, interpolationValue);
                break;
            }
            case Interpolation::Linear: {
                node->scale = LinearV3(previousTimeIndex, nextTimeIndex, interpolationValue);
                break;
            }
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    const glm::vec3 Track::LinearV3(int previousTimeIndex, int nextTimeIndex, float interpolationValue) const
    {
        auto prevValueOffset = previousTimeIndex * 3;
        auto nextValueOffset = nextTimeIndex * 3;

        glm::vec3 prev{ values[prevValueOffset + 0], values[prevValueOffset + 1], values[prevValueOffset + 2] };
        glm::vec3 next{ values[nextValueOffset + 0], values[nextValueOffset + 1], values[nextValueOffset + 2] };

        return glm::mix(prev, next, interpolationValue);
    }

    const glm::quat Track::LinearQ(int previousTimeIndex, int nextTimeIndex, float interpolationValue) const
    {
        auto prevValueOffset = previousTimeIndex * 4;
        auto nextValueOffset = nextTimeIndex * 4;

        glm::quat prev{ values[prevValueOffset + 3], values[prevValueOffset + 0], values[prevValueOffset + 1], values[prevValueOffset + 2] };
        glm::quat next{ values[nextValueOffset + 3], values[nextValueOffset + 0], values[nextValueOffset + 1], values[nextValueOffset + 2] };

        return glm::slerp(prev, next, interpolationValue);
    }

    const glm::vec3 Track::Cubic(int previousTimeIndex, int nextTimeIndex, float deltaTime, float interpolationValue) const
    {
        auto prevValueOffset = previousTimeIndex * 3 * 3;
        auto nextValueOffset = nextTimeIndex * 3 * 3;

        // Tangents (ak, bk) and values (vk) are grouped within keyframes: a1, a2, ..., an, v1, v2, ..., vn, b1, b2, ..., bn)
        auto a0 = glm::make_vec3(values.data() + (prevValueOffset + 0));
        auto v0 = glm::make_vec3(values.data() + (prevValueOffset + 3));
        auto b0 = glm::make_vec3(values.data() + (prevValueOffset + 6));

        auto a1 = glm::make_vec3(values.data() + (nextValueOffset + 0));
        auto v1 = glm::make_vec3(values.data() + (nextValueOffset + 3));
        auto b1 = glm::make_vec3(values.data() + (nextValueOffset + 6));

        return glm::hermite(v0, b0, v1, a1, interpolationValue);
    }

    const glm::quat Track::CubicQ(int previousTimeIndex, int nextTimeIndex, float deltaTime, float interpolationValue) const
    {
        auto prevValueOffset = previousTimeIndex * 3 * 4;
        auto nextValueOffset = nextTimeIndex * 3 * 4;

        // Tangents (ak, bk) and values (vk) are grouped within keyframes: a1, a2, ..., an, v1, v2, ..., vn, b1, b2, ..., bn)
        auto a0 = glm::make_vec4(values.data() + (prevValueOffset + 0));
        auto v0 = glm::make_vec4(values.data() + (prevValueOffset + 4));
        auto b0 = glm::make_vec4(values.data() + (prevValueOffset + 8));

        auto a1 = glm::make_vec4(values.data() + (nextValueOffset + 0));
        auto v1 = glm::make_vec4(values.data() + (nextValueOffset + 4));
        auto b1 = glm::make_vec4(values.data() + (nextValueOffset + 8));

        // https://github.com/KhronosGroup/glTF/issues/1386
        const auto v4 = glm::hermite(v0, b0, v1, a1, interpolationValue);
        return glm::normalize(glm::quat{ v4.w, v4.x, v4.y, v4.z });
    }

    void Animation::Animate(float t, opengl_starter::Node* node)
    {
        for (auto& track : tracks)
            track.Animate(t, node);
    }
}
