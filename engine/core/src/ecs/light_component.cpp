#include "../../include/ecs/light_component.h"



engine::LightComponent::LightComponent(std::shared_ptr<Light> light)
    : m_light(light)
{
}

void engine::LightComponent::init()
{

}

void engine::LightComponent::update()
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
