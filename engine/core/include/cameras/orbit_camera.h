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
        // constructor
        OrbitCamera(glm::vec3 _target = glm::vec3(0.0f, 0.0f, 0.0f), float _radius = 10.0f, float _theta = 90.0f, float _phi = 0.0f, glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f));
        ~OrbitCamera() = default;

        CameraType getTypeID() const override
        {
            return CameraType::orbit;
        }

        ordered_map<std::string, EditorProperty> getPublicProperties() override {
            return {
                {"target", EditorProperty { "Target", getTarget(), editable, -180.0f, 180.0f, 1.0f, "%.2f" }},
                {"radius", EditorProperty { "Radius", getRadius(), editable, 0.0f, 100.0f, 1.0f, "%.2f" }},
                {"theta", EditorProperty { "Theta", getTheta(), editable, -180.0f, 180.0f, 1.0f, "%.2f" }},
                {"phi", EditorProperty { "Phi", getPhi(), editable, -180.0f, 180.0f, 1.0f, "%.2f" }},
                
            };
        }
        std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() override {
            return {
                { "target", [this](EditorPropertyValue value) { getTarget() = *(std::get_if<glm::vec3>(&value)); } },
                { "radius", [this](EditorPropertyValue value) { getRadius() = *(std::get_if<float>(&value)); } },
                { "theta", [this](EditorPropertyValue value) { getTheta() = *(std::get_if<float>(&value)); } },
                { "phi", [this](EditorPropertyValue value) { getPhi() = *(std::get_if<float>(&value)); } }
            };
        }

        float& getRadius() { return m_radius; }
        void setRadius(float _radius) { m_radius = _radius; }

        float& getTheta() { return m_theta; }
        void setTheta(float _theta) { m_theta = _theta; }

        float& getPhi() { return m_phi; }
        void setPhi(float _phi) { m_phi = _phi; }

        glm::vec3& getTarget() { return m_target; }
        void setTarget(glm::vec3 target) { m_target = target; }

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
        glm::vec3 m_target{};
        
        // spherical coordinates
        float m_radius{};
        float m_theta{};   // horizontal angle
        float m_phi{};     // vertical angle

        
        // calculates the camera's position and orientation based on spherical coordinates
        void updateCameraVectors() override;

        void setup() override;
    };
}