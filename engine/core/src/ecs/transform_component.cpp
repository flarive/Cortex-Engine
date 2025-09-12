#include "../../include/ecs/transform_component.h"


engine::TransformComponent::TransformComponent(const Transform& transform) : m_transform(transform)
{
}


void engine::TransformComponent::init()
{

}

void engine::TransformComponent::update(Transform& transform)
{

}

void engine::TransformComponent::draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& transform)
{

}

std::unique_ptr<engine::AABB> engine::TransformComponent::getBoundingVolume()
{
    auto aabb = engine::AABB(glm::vec3(), glm::vec3());
    return std::make_unique<AABB>(aabb);
}
