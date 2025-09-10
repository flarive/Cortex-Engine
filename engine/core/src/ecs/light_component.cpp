#include "../../include/ecs/light_component.h"

#include "../../include/lights/light.h"

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

void engine::LightComponent::draw(Shader& shader, const glm::mat4& transform)
{

}
