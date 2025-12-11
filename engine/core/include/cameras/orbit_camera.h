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
        float radius;
        float theta;   // horizontal angle
        float phi;     // vertical angle

        // constructor
        OrbitCamera(glm::vec3 _target = glm::vec3(0.0f, 0.0f, 0.0f), float _radius = 10.0f, float _theta = 90.0f, float _phi = 0.0f, glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f));
        

        CameraType getTypeID() const override
        {
            return CameraType::orbit;
        }

        ordered_map<std::string, EditorProperty> getPublicProperties() override {
            return {
                {"radius", EditorProperty { getRadius(), 0.0f, 100.0f, 1.0f, "%.2f" }},
                {"theta", EditorProperty { getTheta(), -180.0f, 180.0f, 1.0f, "%.2f" }},
                {"phi", EditorProperty { getPhi(), -180.0f, 180.0f, 1.0f, "%.2f" }}
            };
        }
        std::unordered_map<std::string, std::function<void(float)>> getPropertySetters() override {
            return {
                {"radius", [this](float value) { getRadius() = value; }},
                {"theta", [this](float value) { getTheta() = value; }},
                {"phi", [this](float value) { getPhi() = value; }}
            };
        }

        float& getRadius() { return radius; }
        void setRadius(float _radius) { radius = _radius; }

        float& getTheta() { return theta; }
        void setTheta(float _theta) { theta = _theta; }

        float& getPhi() { return phi; }
        void setPhi(float _phi) { phi = _phi; }

        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
        glm::mat4 getViewMatrix();

        // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        void processKeyboard(CameraMovement direction, float deltaTime, GLboolean constrainPitch = true) override;

        // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPhi = true) override;

        // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void processMouseScroll(float yoffset) override;

        // processes input received from a gamepad
        void processJoystickMovement(const GLFWgamepadstate& state) override;


    private:
        // calculates the camera's position and orientation based on spherical coordinates
        void updateCameraVectors() override;

        void setup() override;
    };
}