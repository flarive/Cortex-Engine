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
