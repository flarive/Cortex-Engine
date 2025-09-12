#include "../../include/ecs/light_component.h"



engine::LightComponent::LightComponent(std::shared_ptr<Light> light)
    : m_light(light)
{
    m_boundingVolume = std::make_unique<AABB>(generateAABB(light));
}

void engine::LightComponent::init()
{

}

void engine::LightComponent::update(Transform& transform)
{

}

void engine::LightComponent::draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& transform)
{
    m_light->draw(shader,
        projection,
        view,
        m_light->ambientColor,
        m_light->diffuseColor,
        m_light->specularColor,
        m_light->intensity,
        m_light->target,
        transform);
}

engine::AABB engine::LightComponent::generateAABB(const std::shared_ptr<Light> light)
{
    glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

    return engine::AABB(minAABB, maxAABB);
}

std::unique_ptr<engine::AABB> engine::LightComponent::getBoundingVolume()
{
    return m_boundingVolume;
}