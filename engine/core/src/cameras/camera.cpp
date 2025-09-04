#include "../../include/cameras/camera.h"

engine::Camera::Camera(glm::vec3 _position, glm::vec3 _up, float _yaw, float _pitch)
    : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
    position = _position;
    worldUp = _up;
    yaw = _yaw;
    pitch = _pitch;
    updateCameraVectors();
}

glm::mat4 engine::Camera::getViewMatrix()
{
    return glm::lookAt(position, position + front, up);
}

void engine::Camera::setFromViewMatrix(const glm::mat4& view)
{
    // Inverse view => world transform
    glm::mat4 inv = glm::inverse(view);
    position = glm::vec3(inv[3]);              // translation
    front = glm::normalize(-glm::vec3(view[0][2], view[1][2], view[2][2])); // forward
    up = glm::normalize(glm::vec3(view[0][1], view[1][1], view[2][1]));  // up
}

void engine::Camera::draw(const glm::vec3& _position)
{
    //position = glm::vec3(_position);

    //updateCameraVectors();
}

void engine::Camera::updateCameraVectors()
{
}