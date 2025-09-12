#include "../../include/ecs/component.h"


std::unique_ptr<engine::AABB> engine::Component::getBoundingVolume()
{
    glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

    return engine::AABB(minAABB, maxAABB);
}

