//#pragma once
//
//#include "misc/noncopyable.h"
//#include "common_defines.h"
//
//namespace engine
//{
//    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
//    enum Camera_Movement {
//        FORWARD,
//        BACKWARD,
//        LEFT,
//        RIGHT,
//        UP,
//        DOWN,
//        YAW_UP,
//        YAW_DOWN,
//        PITCH_UP,
//        PITCH_DOWN
//    };
//
//    // Default camera values
//    const float YAW{ -90.0f };
//    const float PITCH{ 0.0f };
//    const float SPEED{ 2.5f };
//    const float SENSITIVITY{ 0.3f };
//    const float ZOOM{ 45.0f };
//
//
//    // A fly/fps camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
//	class Camera// : private NonCopyable
//    {
//    public:
//        // camera Attributes
//        //std::string name{};
//        glm::vec3 position{};
//        glm::vec3 front{};
//        glm::vec3 up{};
//        glm::vec3 right{};
//        glm::vec3 worldUp{};
//        // euler Angles
//        float yaw{};
//        float pitch{};
//        // camera options
//        float movementSpeed{};
//        float mouseSensitivity{};
//        float zoom{};
//
//        bool fps{};
//
//
//
//
//        // constructor with vectors
//        Camera(glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f), bool _fps = false, glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f), float _yaw = YAW, float _pitch = PITCH);
//
//        // constructor with scalar values
//        Camera(float _posX, float _posY, float _posZ, float _upX, float _upY, float _upZ, float _yaw, float _pitch, bool _fps);
//
//
//        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
//        glm::mat4 GetViewMatrix();
//
//        // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
//        void ProcessKeyboard(Camera_Movement direction, float deltaTime, GLboolean constrainPitch = true);
//        
//        // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
//        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
//       
//        // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
//        void ProcessMouseScroll(float yoffset);
//
//        void ProcessJoystickMovement(const GLFWgamepadstate& state);
//
//    private:
//        // calculates the front vector from the Camera's (updated) Euler Angles
//        void updateCameraVectors();
//    };
//}




#pragma once

#include "misc/noncopyable.h"
#include "common_defines.h"

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
        glm::vec3 Position{};
        glm::vec3 Front{};
        glm::vec3 Up{};
        glm::vec3 Right{};
        glm::vec3 WorldUp{};
        // euler Angles
        float Yaw{};
        float Pitch{};
        // camera options
        float MovementSpeed{};
        float MouseSensitivity{};
        float Zoom{};

        bool Fps{ false };




        // constructor with vectors
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), bool fps = false, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

        // constructor with scalar values
        Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, bool fps);


        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
        glm::mat4 GetViewMatrix();

        // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        void ProcessKeyboard(Camera_Movement direction, float deltaTime, GLboolean constrainPitch = true);

        // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

        // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void ProcessMouseScroll(float yoffset);

        void ProcessJoystickMovement(const GLFWgamepadstate& state);

    private:
        // calculates the front vector from the Camera's (updated) Euler Angles
        void updateCameraVectors();
    };
}