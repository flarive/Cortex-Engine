#include "../include/transform.h"

using namespace engine;

Transform::Transform(glm::vec3 pos, glm::vec3 scale, glm::vec3 rot)
    : m_pos(pos), m_scale(scale), m_eulerRot(rot) {
}

glm::mat4 Transform::getLocalModelMatrix() const {
    glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), glm::radians(m_eulerRot.x), { 1,0,0 });
    glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians(m_eulerRot.y), { 0,1,0 });
    glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_eulerRot.z), { 0,0,1 });
    glm::mat4 rotation = rotY * rotX * rotZ;

    return glm::translate(glm::mat4(1.0f), m_pos)
        * rotation
        * glm::scale(glm::mat4(1.0f), m_scale);
}

glm::vec3 Transform::getRight(const glm::mat4& world) const { return glm::vec3(world[0]); }
glm::vec3 Transform::getUp(const glm::mat4& world) const { return glm::vec3(world[1]); }
glm::vec3 Transform::getBackward(const glm::mat4& world) const { return glm::vec3(world[2]); }
glm::vec3 Transform::getForward(const glm::mat4& world) const { return -glm::vec3(world[2]); }

glm::vec3 Transform::getGlobalScale(const glm::mat4& world) const
{
    return { glm::length(getRight(world)),
             glm::length(getUp(world)),
             glm::length(getBackward(world)) };
}