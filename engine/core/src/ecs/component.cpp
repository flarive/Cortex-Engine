#include "../../include/ecs/component.h"

engine::AABB* engine::Component::getBoundingVolume()
{
    auto aabb = engine::AABB(glm::vec3(), glm::vec3());
    auto zzz = std::make_unique<AABB>(aabb);
    return zzz.get();
}