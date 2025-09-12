#include "../../include/ecs/component.h"


std::unique_ptr<engine::AABB> engine::Component::getBoundingVolume()
{
    auto aabb = engine::AABB(glm::vec3(), glm::vec3());
    return std::make_unique<AABB>(aabb);
}