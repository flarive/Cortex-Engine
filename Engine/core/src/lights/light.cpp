#include "../../include/lights/light.h"

#include "../../include/transform.h"

engine::Light::Light(unsigned int index) : position(glm::vec3()), m_index(index)
{
}

engine::Light::Light(glm::vec3 _position, unsigned int index) : position(_position), m_index(index)
{
}

glm::vec3 engine::Light::calculateLightDirection(const glm::vec3& position, const glm::vec3& target)
{
    return glm::normalize(target - position);
}