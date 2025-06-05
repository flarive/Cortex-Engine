#pragma once

#include "camera.h"

namespace engine
{
    // A fly/fps camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
    class FlyCamera final : public Camera
    {
    public:
        // constructor with vectors
        FlyCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), bool fps = false, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

        // constructor with scalar values
        FlyCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, bool fps);


        

        // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        void processKeyboard(Camera_Movement direction, float deltaTime, GLboolean constrainPitch = true) override;

        // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) override;

        // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void processMouseScroll(float yoffset) override;

        void processJoystickMovement(const GLFWgamepadstate& state) override;
    };
}