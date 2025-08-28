#include "../../include/cameras/orbit_camera.h"

engine::OrbitCamera::OrbitCamera(glm::vec3 _target, float _distance, float _yaw, float _pitch)
    : Camera(false, glm::vec3(0.0f, 1.0f, 0.0f), _yaw, _pitch), target(_target), distance(_distance)
{
    //updateCameraVectors();
}


// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void engine::OrbitCamera::processKeyboard(engine::Camera_Movement direction, float deltaTime, GLboolean constrainPitch)
{
    // Optional: zoom in/out or pan the target
    float velocity = movementSpeed * deltaTime;
    if (direction == FORWARD) distance -= velocity;
    if (direction == BACKWARD) distance += velocity;
    if (direction == LEFT) target -= right * velocity;
    if (direction == RIGHT) target += right * velocity;
    if (direction == UP) target += up * velocity;
    if (direction == DOWN) target -= up * velocity;

    updateCameraVectors();
}

void engine::OrbitCamera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (constrainPitch)
    {
        pitch = glm::clamp(pitch, -89.0f, 89.0f);
    }

    updateCameraVectors();
}

void engine::OrbitCamera::processMouseScroll(float yoffset)
{
    distance -= yoffset;
    distance = glm::max(distance, 0.1f); // prevent flipping
    updateCameraVectors();
}

void engine::OrbitCamera::processJoystickMovement(const GLFWgamepadstate& state)
{
    // Optional: implement joystick orbiting or zoom
}

void engine::OrbitCamera::updateCameraVectors()
{
    // Calculate the new position based on spherical coordinates
    float radPitch = glm::radians(pitch);
    float radYaw = glm::radians(yaw);

    position.x = target.x + distance * cos(radPitch) * cos(radYaw);
    position.y = target.y + distance * sin(radPitch);
    position.z = target.z + distance * cos(radPitch) * sin(radYaw);

    front = glm::normalize(target - position);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}