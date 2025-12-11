#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"

#include "../transform.h"

#include "../frustrum.h"
#include "../misc/ordered_map.h"

#include <vector>
#include <map>
#include <unordered_map>
#include <functional>

namespace engine
{
    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum CameraMovement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN,
        YAW_UP,
        YAW_DOWN,
        PITCH_UP,
        PITCH_DOWN
    };

    // Default camera values
    const float YAW{ -90.0f };
    const float PITCH{ 0.0f };
    const float SPEED{ 2.5f };
    const float SENSITIVITY{ 0.3f };
    const float ZOOM{ 45.0f };

    enum class CameraType { undefined = 0, fps = 1, fly = 2, orbit = 3 };

    const std::unordered_map<CameraType, std::string> CameraTypeNames = {
        {CameraType::undefined, "undefined"},
        {CameraType::fps, "FPS"},
        {CameraType::fly, "Fly"},
        {CameraType::orbit, "Orbit"}
    };

    inline std::string to_string(CameraType type) {
        auto it = CameraTypeNames.find(type);
        return it != CameraTypeNames.end() ? it->second : "unknown";
    }



    // A fly/fps camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
    class Camera// : private NonCopyable
    {
    public:
        // camera Attributes
        glm::vec3 position{};
        glm::vec3 front{};
        glm::vec3 up{};
        glm::vec3 right{};
        glm::vec3 worldUp{};
        
        // euler Angles
        float yaw{};
        float pitch{};
        
        // camera options
        float movementSpeed{};
        float mouseSensitivity{};
        float zoom{}; // fov

        // constructor with vectors
        Camera(glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f), float _zoom = ZOOM, float _yaw = YAW, float _pitch = PITCH, float _speed = SPEED, float _sensitivity = SENSITIVITY);

        virtual CameraType getTypeID() const
        {
            return CameraType::undefined;
        }

        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
        glm::mat4 getViewMatrix();

        void setFromViewMatrix(const glm::mat4& view);

        // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        virtual void processKeyboard(CameraMovement direction, float deltaTime, GLboolean constrainPitch = true) = 0;

        // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        virtual void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) = 0;

        // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        virtual void processMouseScroll(float yoffset) = 0;

        virtual void processJoystickMovement(const GLFWgamepadstate& state) = 0;

        virtual ordered_map<std::string, EditorProperty> getPublicProperties() = 0;
        virtual std::unordered_map<std::string, std::function<void(float)>> getPropertySetters() = 0;

		virtual void setup() = 0;

        void draw(const glm::vec3& _position);

        Frustum createFrustumFromCamera(float aspect, float fovY, float zNear, float zFar);

        void reSetup() { setup(); };

    protected:
        // calculates the front vector from the Camera's (updated) Euler Angles
        virtual void updateCameraVectors() = 0;
    };
}