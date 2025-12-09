#include "../../include/cameras/orbit_camera.h"

engine::OrbitCamera::OrbitCamera(glm::vec3 _target, float _radius, float _theta, float _phi, glm::vec3 _up)
    : engine::Camera(glm::vec3(0.0, 0.0, 0.0), _up), target(_target), Radius(_radius), Theta(_theta), Phi(_phi)
{
    updateCameraVectors();
}

void engine::OrbitCamera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPhi)
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    Theta += xoffset;
    Phi += yoffset;

    // make sure that when phi is out of bounds, screen doesn't get flipped
    if (constrainPhi)
    {
        if (Phi > 89.0f)
            Phi = 89.0f;
        if (Phi < -89.0f)
            Phi = -89.0f;
    }

    updateCameraVectors();
}

void engine::OrbitCamera::processMouseScroll(float yoffset)
{
    Radius -= yoffset;
    if (Radius < 1.0f)
        Radius = 1.0f;
}

void engine::OrbitCamera::processKeyboard(Camera_Movement direction, float deltaTime, GLboolean constrainPitch)
{
    (void)direction;   //Do nothing
    (void)deltaTime;   //Do nothing
    (void)constrainPitch;   //Do nothing
}

void engine::OrbitCamera::processJoystickMovement(const GLFWgamepadstate& state)
{
    (void)state;   //Do nothing
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 engine::OrbitCamera::getViewMatrix()
{
    return glm::lookAt(position, target, up);
}

void engine::OrbitCamera::updateCameraVectors()
{
    // spherical to cartesian coordinates
    position.x = target.x + Radius * sin(glm::radians(Theta)) * cos(glm::radians(Phi));
    position.y = target.y + Radius * sin(glm::radians(Phi));
    position.z = target.z + Radius * cos(glm::radians(Theta)) * cos(glm::radians(Phi));

    // calculate the new Front vector
    front = glm::normalize(target - position);
    // also re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}


