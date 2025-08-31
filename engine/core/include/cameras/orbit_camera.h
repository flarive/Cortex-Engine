#pragma once

#include "camera.h"

namespace engine
{
    class OrbitCamera : public Camera
    {
    public:
        glm::vec3 target{ 0.0f, 0.0f, 0.0f };
        //float distance{ 10.0f };

        // spherical coordinates
        float Radius;
        float Theta;   // horizontal angle
        float Phi;     // vertical angle

        // constructor
        OrbitCamera(glm::vec3 _target = glm::vec3(0.0f, 0.0f, 0.0f), float _radius = 10.0f, float _theta = 90.0f, float _phi = 0.0f, glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f))
            : engine::Camera(glm::vec3(0.0, 0.0, 0.0), false), target(_target), Radius(_radius), Theta(_theta), Phi(_phi)
        {
            updateCameraVectors();
        }

        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
        glm::mat4 getViewMatrix()
        {
            return glm::lookAt(position, target, up);
        }

        // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPhi = true)
        {
            xoffset *= mouseSensitivity;
            yoffset *= mouseSensitivity;

            Theta += xoffset;
            Phi += yoffset;

            // make sure that when phi is out of bounds, screen doesn't get flipped
            if (constrainPhi)
            {
                if (Phi > 89.0f)
                    Phi = 89.0f;
                if (Phi < -89.0f)
                    Phi = -89.0f;
            }

            updateCameraVectors();
        }

        // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void processMouseScroll(float yoffset)
        {
            Radius -= yoffset;
            if (Radius < 1.0f)
                Radius = 1.0f;
        }


        void processKeyboard(Camera_Movement direction, float deltaTime, GLboolean constrainPitch = true)
        {

        }


        void processJoystickMovement(const GLFWgamepadstate& state)
        {

        }


    private:
        // calculates the camera's position and orientation based on spherical coordinates
        void updateCameraVectors()
        {
            // spherical to cartesian coordinates
            position.x = target.x + Radius * sin(glm::radians(Theta)) * cos(glm::radians(Phi));
            position.y = target.y + Radius * sin(glm::radians(Phi));
            position.z = target.z + Radius * cos(glm::radians(Theta)) * cos(glm::radians(Phi));

            // calculate the new Front vector
            front = glm::normalize(target - position);
            // also re-calculate the Right and Up vector
            right = glm::normalize(glm::cross(front, worldUp));
            up = glm::normalize(glm::cross(right, front));
        }
    };
}