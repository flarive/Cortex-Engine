#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"

namespace engine
{
    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum Camera_Movement {
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
        Camera(glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f), float _yaw = YAW, float _pitch = PITCH);

        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
        glm::mat4 getViewMatrix();

        void setFromViewMatrix(const glm::mat4& view);



        // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        virtual void processKeyboard(Camera_Movement direction, float deltaTime, GLboolean constrainPitch = true) = 0;

        // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        virtual void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) = 0;

        // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        virtual void processMouseScroll(float yoffset) = 0;

        virtual void processJoystickMovement(const GLFWgamepadstate& state) = 0;




    protected:
        // calculates the front vector from the Camera's (updated) Euler Angles
        virtual void updateCameraVectors() = 0;
    };
}