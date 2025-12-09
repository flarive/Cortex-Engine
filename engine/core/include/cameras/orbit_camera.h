#pragma once

#include "camera.h"

namespace engine
{
    /// <summary>
    /// An orbit camera that turn around a target point
    /// </summary>
    class OrbitCamera final : public Camera
    {
    public:
        glm::vec3 target{ 0.0f, 0.0f, 0.0f };

        // spherical coordinates
        float Radius;
        float Theta;   // horizontal angle
        float Phi;     // vertical angle

        // constructor
        OrbitCamera(glm::vec3 _target = glm::vec3(0.0f, 0.0f, 0.0f), float _radius = 10.0f, float _theta = 90.0f, float _phi = 0.0f, glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f));
        

        CameraType getTypeID() const override
        {
            return CameraType::orbit;
        }

        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
        glm::mat4 getViewMatrix();

        // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        void processKeyboard(Camera_Movement direction, float deltaTime, GLboolean constrainPitch = true) override;

        // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPhi = true) override;

        // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void processMouseScroll(float yoffset) override;

        // processes input received from a gamepad
        void processJoystickMovement(const GLFWgamepadstate& state) override;


    private:
        // calculates the camera's position and orientation based on spherical coordinates
        void updateCameraVectors() override;
    };
}