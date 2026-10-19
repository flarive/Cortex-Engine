#include "../../include/cameras/orbit_camera.h"

#include "../../include/managers/log_manager.h"

engine::OrbitCamera::OrbitCamera(glm::vec3 _target, float _radius, float _theta, float _phi, glm::vec3 _up)
    : engine::Camera(glm::vec3(0.0, 0.0, 0.0), _up), m_target(_target), m_radius(_radius), m_theta(_theta), m_phi(_phi)
{
    logger.trace("OrbitCamera constructor called");

    updateCameraVectors(); // needed if base Camera constructor is called
}

void engine::OrbitCamera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPhi)
{
    if (!m_enabled)
        return;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    m_theta += xoffset;
    m_phi += yoffset;

    // make sure that when phi is out of bounds, screen doesn't get flipped
    if (constrainPhi)
    {
        if (m_phi > 89.0f)
            m_phi = 89.0f;
        if (m_phi < -89.0f)
            m_phi = -89.0f;
    }

    updateCameraVectors();
}

void engine::OrbitCamera::processMouseScroll(float yoffset)
{
    if (!m_enabled)
        return;

    m_radius -= yoffset;
    if (m_radius < 1.0f)
        m_radius = 1.0f;
}

void engine::OrbitCamera::processKeyboard(CameraMovement direction, float deltaTime, GLboolean constrainPitch)
{
    (void)direction;   //Do nothing
    (void)deltaTime;   //Do nothing
    (void)constrainPitch;   //Do nothing

    if (!m_enabled)
        return;

}

void engine::OrbitCamera::processJoystickMovement(float leftX, float leftY, float rightX, float rightY, float triggerL, float triggerR, float deltaTime)
{
    if (!m_enabled)
        return;

    const float velocity = movementSpeed * deltaTime;

    const float rotationSpeed = mouseSensitivity * 100.0f * deltaTime;

    position += front * (-leftY * velocity);
    position += right * (leftX * velocity);
    position -= up * ((triggerL + 1.0f) * 0.5f * velocity);
    position += up * ((triggerR + 1.0f) * 0.5f * velocity);

    yaw += rightX * rotationSpeed;
    pitch -= rightY * rotationSpeed;

    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    updateCameraVectors();
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 engine::OrbitCamera::getViewMatrix()
{
    return glm::lookAt(position, m_target, up);
}

void engine::OrbitCamera::updateCameraVectors()
{
    if (!m_enabled)
        return;

    // spherical to cartesian coordinates
    position.x = m_target.x + m_radius * sin(glm::radians(m_theta)) * cos(glm::radians(m_phi));
    position.y = m_target.y + m_radius * sin(glm::radians(m_phi));
    position.z = m_target.z + m_radius * cos(glm::radians(m_theta)) * cos(glm::radians(m_phi));

    // calculate the new Front vector
    front = glm::normalize(m_target - position);
    // also re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

void engine::OrbitCamera::setup()
{
    updateCameraVectors();
}

engine::OrbitCamera::~OrbitCamera()
{
    logger.trace("OrbitCamera destructor called");
}