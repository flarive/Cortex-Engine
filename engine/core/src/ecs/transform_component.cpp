#include "../../include/ecs/transform_component.h"


engine::TransformComponent::TransformComponent(const Transform& transform) : m_transform(transform)
{
}


void engine::TransformComponent::init(Transform& transform)
{

}

void engine::TransformComponent::update(Transform& transform)
{

}

void engine::TransformComponent::draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform, AABB* boundingVolume)
{

}

engine::AABB* engine::TransformComponent::getBoundingVolume()
{
    auto aabb = engine::AABB(glm::vec3(), glm::vec3());
    auto zzz = std::make_unique<AABB>(aabb);
    return zzz.get();
}

std::vector<engine::KeyValuePair> engine::TransformComponent::getPublicProperties()
{
    return std::vector<engine::KeyValuePair>{};
}