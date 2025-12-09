#pragma once

#include "camera.h"

namespace engine
{
    // A FPS camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
    class FpsCamera final : public Camera
    {
    public:
        // constructor with vectors
        FpsCamera(glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f), float _yaw = YAW, float _pitch = PITCH);

        CameraType getTypeID() const override
        {
            return CameraType::fps;
        }

        // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        void processKeyboard(Camera_Movement direction, float deltaTime, GLboolean constrainPitch = true) override;

        // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) override;

        // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void processMouseScroll(float yoffset) override;

        // processes input received from a gamepad
        void processJoystickMovement(const GLFWgamepadstate& state) override;


    protected:
        void updateCameraVectors() override;
    };
}