#pragma once

#include "transform.h"

namespace engine
{
    enum class AnimMode
    {
        Absolute,
        Relative
    };

    struct AnimTransform final
    {
        AnimTransform() = default;
        AnimTransform(const Transform& from_, const Transform& to_, AnimMode mode_, float duration_, bool loop_ = false);
        ~AnimTransform() = default;
        
        Transform from{};
        Transform to{};
        AnimMode mode = AnimMode::Absolute;

        float duration{ 1.0f }; // s
        float elapsed{};

        bool loop{ false };

        

        // Configure animation
        void setup(const Transform& current,
            const Transform& from_,
            const Transform& to_,
            AnimMode mode_,
            bool loop_ = false);

        // Apply animation each frame
        bool update(float dt, Transform& outTransform);

        std::string timeNow();
    };
}