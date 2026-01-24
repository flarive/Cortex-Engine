#include "../../include/ecs/component.h"

engine::AABB* engine::Component::getBoundingVolume()
{
    return new engine::AABB(glm::vec3(), glm::vec3());
    // Caller must delete the pointer later!
}

void engine::Component::setEnabled(bool enabled)
{
    m_isEnabled = enabled;
}