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
        AnimTransform() = default;
		AnimTransform(const Transform& from_, const Transform& to_, AnimMode mode_, float duration_)
            : from(from_), to(to_), mode(mode_), duration(duration_)//, durationSeconds(duration_ / 1000.0f)
        {
        }
        ~AnimTransform() = default;
        
        Transform from{};
        Transform to{};
        AnimMode mode = AnimMode::Absolute;

        float duration{ 1.0f }; // s
        //float durationSeconds {1.0f}; // s

        float elapsed{};

        

        // Configure animation
        void setup(const Transform& current,
            const Transform& from_,
            const Transform& to_,
            AnimMode mode_);

        // Apply animation each frame
        bool update(float dt, Transform& outTransform);
    };
}