#include "../../include/ecs/light_component.h"



engine::LightComponent::LightComponent(std::shared_ptr<Light> light)
    : m_light(light)
{
    m_boundingVolume = std::make_unique<AABB>(generateBoundingVolume(light));
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

void engine::LightComponent::draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation)
{

}

void engine::LightComponent::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, const Color& diffuse, const Color& specular, float intensity, const glm::vec3& target, const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation)
{
    m_light->draw(shader,
        projection,
        view,
        ambient,
        diffuse,
        specular,
        intensity,
        position,
        target,
        size,
        rotation);
}

engine::AABB engine::LightComponent::generateBoundingVolume(const std::shared_ptr<Light> light)
{
    glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

    return engine::AABB(minAABB, maxAABB);
}

std::unique_ptr<engine::AABB> engine::LightComponent::getBoundingVolume()
{
    return std::move(m_boundingVolume);
}