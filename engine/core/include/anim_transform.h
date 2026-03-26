#pragma once

#include "transform.h"

namespace engine
{
    enum class AnimMode
    {
        Absolute,
        Relative
    };

    struct AnimTransform
    {
        Transform from{};
        Transform to{};
        AnimMode mode = AnimMode::Absolute;

        float duration = 1.0f;
        float elapsed = 0.0f;

        // Configure animation
        void setup(const Transform& current,
            const Transform& from_,
            const Transform& to_,
            AnimMode mode_);

        // Apply animation each frame
        bool update(float dt, Transform& outTransform);
    };
}