#include "../../include/cameras/orbit_camera.h"

engine::OrbitCamera::OrbitCamera(glm::vec3 _target, float _radius, float _theta, float _phi, glm::vec3 _up)
    : engine::Camera(glm::vec3(0.0, 0.0, 0.0), _up), target(_target), radius(_radius), theta(_theta), phi(_phi)
{
    updateCameraVectors(); // needed if base Camera constructor is called
}

void engine::OrbitCamera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPhi)
{
    if (!m_enabled)
        return;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    theta += xoffset;
    phi += yoffset;

    // make sure that when phi is out of bounds, screen doesn't get flipped
    if (constrainPhi)
    {
        if (phi > 89.0f)
            phi = 89.0f;
        if (phi < -89.0f)
            phi = -89.0f;
    }

    updateCameraVectors();
}

void engine::OrbitCamera::processMouseScroll(float yoffset)
{
    if (!m_enabled)
        return;

    radius -= yoffset;
    if (radius < 1.0f)
        radius = 1.0f;
}

void engine::OrbitCamera::processKeyboard(CameraMovement direction, float deltaTime, GLboolean constrainPitch)
{
    (void)direction;   //Do nothing
    (void)deltaTime;   //Do nothing
    (void)constrainPitch;   //Do nothing

    if (!m_enabled)
        return;

}

void engine::OrbitCamera::processJoystickMovement(const GLFWgamepadstate& state)
{
    (void)state;   //Do nothing

    if (!m_enabled)
        return;
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 engine::OrbitCamera::getViewMatrix()
{
    return glm::lookAt(position, target, up);
}

void engine::OrbitCamera::updateCameraVectors()
{
    if (!m_enabled)
        return;

    // spherical to cartesian coordinates
    position.x = target.x + radius * sin(glm::radians(theta)) * cos(glm::radians(phi));
    position.y = target.y + radius * sin(glm::radians(phi));
    position.z = target.z + radius * cos(glm::radians(theta)) * cos(glm::radians(phi));

    // calculate the new Front vector
    front = glm::normalize(target - position);
    // also re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

void engine::OrbitCamera::setup()
{
    updateCameraVectors();
}