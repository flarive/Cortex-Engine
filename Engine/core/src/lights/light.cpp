#include "../../include/lights/light.h"

engine::Light::Light() : m_index(0)
{
}

engine::Light::Light(unsigned int index) : m_index(index)
{
}

glm::vec3 engine::Light::calculateLightDirection(const glm::vec3& position, const glm::vec3& target)
{
    return glm::normalize(target - position);
}

void engine::Light::setIntensity(const float intensity)
{
    m_intensity = intensity;
}

void engine::Light::setAmbientColor(const engine::Color& ambientColor)
{
    m_ambientColor = ambientColor;
}