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
#include <array>

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

    const float MIN_CAMERA_NEAR_PLANE{ 0.1f };
    const float MAX_CAMERA_NEAR_PLANE{ 10.0f };

    const float MIN_CAMERA_FAR_PLANE{ 100.0f };
    const float MAX_CAMERA_FAR_PLANE{ 1000.0f };

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


    struct FrustumCorners {
        std::array<glm::vec3, 8> corners;
    };

    struct Bounds2D {
        float width;
        float height;
    };



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
        

		



        // constructor with vectors
        Camera(glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f), float _zoom = ZOOM, float _yaw = YAW, float _pitch = PITCH, float _speed = SPEED, float _sensitivity = SENSITIVITY);
        virtual ~Camera();


        virtual CameraType getTypeID() const
        {
            return CameraType::undefined;
        }

        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
        glm::mat4& getViewMatrix();

        void setFromViewMatrix(const glm::mat4& view);

        glm::mat4& getProjectionMatrix(float aspect);

        // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        virtual void processKeyboard(CameraMovement direction, float deltaTime, GLboolean constrainPitch = true) = 0;

        // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        virtual void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) = 0;

        // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        virtual void processMouseScroll(float yoffset) = 0;

        virtual void processJoystickMovement(float leftX, float leftY, float rightX, float rightY, float triggerL, float triggerR, float deltaTime) = 0;

        virtual ordered_map<std::string, EditorProperty> getPublicProperties() = 0;
        virtual std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() = 0;

		virtual void setup() = 0;

        void draw(const glm::vec3& _position);

        Frustum createFrustumFromCamera(float aspect, float fovY, float zNear, float zFar);

        Bounds2D getBounds(float aspect, float fovY, float zNear);

        bool checkPlanes();

        // Inside the Camera class definition
        float getDistanceToTarget(const glm::vec3& target) const
        {
            return glm::distance(position, target);
        }

        void reSetup() { setup(); };

        const std::string& getName() { return m_name; }
        void setName(const std::string& name) { m_name = name; }

        bool getEnabled() const { return m_enabled; }
        void setEnabled(bool enabled) { m_enabled = enabled; }

        float& getNearPlane() { return m_nearPlane; }
        float& getFarPlane() { return m_farPlane; }

        void setNearPlane(float near) { m_nearPlane = near; }
        void setFarPlane(float far) { m_farPlane = far; }

        bool getIsPerspective() const { return m_isPerspective; }
        void setIsPerspective(bool is) { m_isPerspective = is; }


        float& getZoom() { return m_zoom; }
        void setZoom(float zoom) { m_zoom = zoom; }


    protected:
        bool m_enabled{ true };

        std::string m_name{};

        bool m_isPerspective{ true };

        float m_zoom{}; // fov

        glm::mat4 m_viewMatrix{}; // Store the view matrix
        glm::mat4 m_projection{};

        // For a standard renderer: 0.1f
        // For first-person games: 0.03f
        // Never goes bellow 0.01f to avoid severe z-fighting issues
        float m_nearPlane{ 0.1f };
        
        // Indoor game: 100.0f
        // Third-person action game: 500.0f;
        // Open-world game: between 2000.0f and 10000.0f;
        float m_farPlane{ 500.0f };

        // far/near ratio
        // 1, 000    excellent
        // 10, 000   good
        // 100, 000  acceptable
        // 1, 000, 000 + dangerous (likely z-fighting)

        // calculates the front vector from the Camera's (updated) Euler Angles
        virtual void updateCameraVectors() = 0;
    };
}