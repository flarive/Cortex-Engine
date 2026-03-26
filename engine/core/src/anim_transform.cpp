#include "../include/anim_transform.h"

void engine::AnimTransform::setup(const Transform& current,
    const Transform& from_,
    const Transform& to_,
    AnimMode mode_)
{
    mode = mode_;
    elapsed = 0.0f;

    if (mode == AnimMode::Absolute)
    {
        from = from_;
        to = to_;
    }
    else // Relative
    {
        // Start from current
        from = current;

        // Convert delta into absolute target
        to.setLocalPosition(current.getLocalPosition() + to_.getLocalPosition());
        to.setLocalRotation(current.getLocalRotation() + to_.getLocalRotation());
        to.setLocalScale(current.getLocalScale() * to_.getLocalScale());
    }
}

bool engine::AnimTransform::update(float dt, Transform& outTransform)
{
    elapsed += dt;

    float t = glm::clamp(elapsed / duration, 0.0f, 1.0f);

    // Interpolate TRS
    outTransform.setLocalPosition(
        glm::mix(from.getLocalPosition(), to.getLocalPosition(), t)
    );

    outTransform.setLocalRotation(
        glm::mix(from.getLocalRotation(), to.getLocalRotation(), t)
    );

    outTransform.setLocalScale(
        glm::mix(from.getLocalScale(), to.getLocalScale(), t)
    );

    return (t >= 1.0f); // finished?
}