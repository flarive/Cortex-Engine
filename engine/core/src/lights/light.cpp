#include "../../include/lights/light.h"

engine::Light::Light() : m_position(glm::vec3())
{
}

engine::Light::Light(glm::vec3 _position) : m_position(_position)
{
}

glm::vec3 engine::Light::calculateLightDirection(const glm::vec3& position, const glm::vec3& target)
{
    return glm::normalize(target - position);
}