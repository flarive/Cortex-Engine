#pragma once

#include "camera.h"

namespace engine
{
    class OrbitCamera : public Camera
    {
    public:
        glm::vec3 target{ 0.0f, 0.0f, 0.0f };
        float distance{ 10.0f };

        OrbitCamera(glm::vec3 _target = glm::vec3(0.0f), float _distance = 10.0f, float _yaw = YAW, float _pitch = PITCH);

        glm::mat4 getViewMatrix()
        {
            return glm::lookAt(position, target, up);
        }

        void processKeyboard(Camera_Movement direction, float deltaTime, GLboolean constrainPitch = true) override;

        void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) override;


        void processMouseScroll(float yoffset) override;


        void processJoystickMovement(const GLFWgamepadstate& state) override;
        

    protected:
        void updateCameraVectors() override;
    };
}