#include "../../include/cameras/camera.h"

engine::Camera::Camera(glm::vec3 _position, bool _fps, glm::vec3 up, float _yaw, float _pitch)
    : fps(_fps), front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
    position = position;
    worldUp = up;
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

glm::mat4 engine::Camera::GetViewMatrix()
{
    return glm::lookAt(position, position + front, up);
}

void engine::Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    // also re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    up = glm::normalize(glm::cross(right, front));
}