#include "../../include/ecs/primitive_component.h"

#include "../../include/primitives/primitive.h"

engine::PrimitiveComponent::PrimitiveComponent(std::shared_ptr<Primitive> primitive)
    : m_primitive(primitive)
{
}

void engine::PrimitiveComponent::init()
{

}

void engine::PrimitiveComponent::update()
{

}

void engine::PrimitiveComponent::draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& transform)
{
    m_primitive->draw(shader, transform);
}
