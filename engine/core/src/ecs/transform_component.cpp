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

void engine::TransformComponent::draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& transform)
{

}

void engine::TransformComponent::draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation)
{

}

engine::AABB* engine::TransformComponent::getBoundingVolume()
{
    auto aabb = engine::AABB(glm::vec3(), glm::vec3());
    auto zzz = std::make_unique<AABB>(aabb);
    return zzz.get();
}
