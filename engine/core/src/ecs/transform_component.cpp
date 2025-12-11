#include "../../include/ecs/transform_component.h"


engine::TransformComponent::TransformComponent(const Transform& transform) : m_transform(transform)
{
}


void engine::TransformComponent::init(Transform& transform)
{

}

void engine::TransformComponent::update(float deltaTime, Transform& transform)
{

}

void engine::TransformComponent::draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform, AABB* boundingVolume)
{

}

engine::AABB* engine::TransformComponent::getBoundingVolume()
{
    return new engine::AABB(glm::vec3(), glm::vec3());
    // Caller must delete the pointer later!
}

engine::ordered_map<std::string, engine::EditorProperty> engine::TransformComponent::getPublicProperties()
{
    return engine::ordered_map<std::string, EditorProperty>{};
}

std::unordered_map<std::string, std::function<void(float)>> engine::TransformComponent::getPropertySetters()
{
    return std::unordered_map<std::string, std::function<void(float)>>();
}

void engine::TransformComponent::setProperty(const std::string& key, float value)
{
}