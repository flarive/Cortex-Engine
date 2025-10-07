#include "../../include/ecs/light_component.h"



engine::LightComponent::LightComponent(std::shared_ptr<Light> light)
    : m_light(light)
{
    m_boundingVolume = std::make_unique<AABB>(generateBoundingVolume(light));
}

void engine::LightComponent::init(Transform& transform)
{
    m_light->position = transform.getLocalPosition();
}

void engine::LightComponent::update(Transform& transform)
{

}

void engine::LightComponent::draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform, AABB* boundingVolume)
{
    m_light->draw(shader,
        projection,
        view,
        m_light->ambientColor,
        m_light->diffuseColor,
        m_light->specularColor,
        m_light->intensity,
        m_light->target,
        worldTransformMatrix);
}

engine::AABB engine::LightComponent::generateBoundingVolume(const std::shared_ptr<Light> light)
{
    glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

    return engine::AABB(minAABB, maxAABB);
}

engine::AABB* engine::LightComponent::getBoundingVolume()
{
    return m_boundingVolume.get();
}

std::vector<engine::KeyValuePair> engine::LightComponent::getPublicProperties()
{
    return std::vector<engine::KeyValuePair>{};
}