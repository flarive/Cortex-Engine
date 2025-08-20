#include "../../include/cameras/camera.h"

engine::Camera::Camera(bool _fps, glm::vec3 _up, float _yaw, float _pitch)
    : fps(_fps), front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
    worldUp = _up;
    yaw = _yaw;
    pitch = _pitch;
    updateCameraVectors();
}

engine::Camera::Camera(glm::vec3 _position, bool _fps, glm::vec3 _up, float _yaw, float _pitch)
    : fps(_fps), front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
    position = _position;
    worldUp = _up;
    yaw = _yaw;
    pitch = _pitch;
    updateCameraVectors();
}

// constructor with scalar values
engine::Camera::Camera(float _posX, float _posY, float _posZ, float _upX, float _upY, float _upZ, float _yaw, float _pitch, bool _fps)
    : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM), fps(_fps)
{
    position = glm::vec3(_posX, _posY, _posZ);
    worldUp = glm::vec3(_upX, _upY, _upZ);
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


void engine::Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 new_front;
    new_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    new_front.y = sin(glm::radians(pitch));
    new_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(new_front);

    // also re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    up = glm::normalize(glm::cross(right, new_front));
}