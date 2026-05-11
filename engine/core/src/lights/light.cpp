#include "../../include/lights/light.h"

#include "../../include/managers/log_manager.h"


engine::Light::Light() : m_position(glm::vec3())
{
    logger.trace("Light base constructor called");
}

engine::Light::Light(glm::vec3 _position) : m_position(_position)
{
    logger.trace("Light base constructor called");
}

glm::vec3 engine::Light::calculateLightDirection(const glm::vec3& position, const glm::vec3& target)
{
    return glm::normalize(target - position);
}

engine::Light::~Light()
{
    logger.trace("Light base destructor called");
}
