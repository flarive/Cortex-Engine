#pragma once

#include "camera.h"

namespace engine
{
    // A fly camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
    class FlyCamera final : public Camera
    {
    public:
        // constructor with vectors
        FlyCamera(float _zoom = ZOOM, float _yaw = YAW, float _pitch = PITCH, float _speed = SPEED, float _sensitivity = SENSITIVITY, glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f));

        CameraType getTypeID() const override
        {
            return CameraType::fly;
        }

        ordered_map<std::string, EditorProperty> getPublicProperties() override {
            return {
                {"Zoom", EditorProperty { getZoom(), 0.0f, 100.0f, 1.0f, "%.2f" }},
                {"Yaw", EditorProperty { getYaw(), -180.0f, 180.0f, 1.0f, "%.2f" }},
                {"Pitch", EditorProperty { getPitch(), -90.0f, 90.0f, 1.0f, "%.2f" }}
            };
        }
        std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() override {
            return {
                /*{"zoom", [this](float value) { getZoom() = value; }},
                {"yaw", [this](float value) { getYaw() = value; }},
                {"pitch", [this](float value) { getPitch() = value; }}*/
                { "zoom", [this](EditorPropertyValue value) { getZoom() = *(std::get_if<float>(&value)); } },
                { "yaw", [this](EditorPropertyValue value) { getYaw() = *(std::get_if<float>(&value)); } },
                { "pitch", [this](EditorPropertyValue value) { getPitch() = *(std::get_if<float>(&value)); } }
                
            };
        }


        float& getZoom() { return zoom; }
        void setZoom(float _zoom) { zoom = _zoom; }

        float& getYaw() { return yaw; }
        void setYaw(float _yaw) { yaw = _yaw; }

        float& getPitch() { return pitch; }
        void setPitch(float _pitch) { pitch = _pitch; }

        // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        void processKeyboard(CameraMovement direction, float deltaTime, GLboolean constrainPitch = true) override;

        // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) override;

        // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void processMouseScroll(float yoffset) override;

        // processes input received from a gamepad
        void processJoystickMovement(const GLFWgamepadstate& state) override;

        void setup() override;



    protected:
        void updateCameraVectors() override;
    };
}