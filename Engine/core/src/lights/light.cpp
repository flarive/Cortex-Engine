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