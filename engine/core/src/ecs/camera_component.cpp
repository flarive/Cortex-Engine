#include "../../include/ecs/camera_component.h"

#include "../../include/cameras/camera.h"

engine::CameraComponent::CameraComponent(std::shared_ptr<Camera> camera)
    : m_camera(camera)
{
}


void engine::CameraComponent::init()
{

}

void engine::CameraComponent::update()
{

}

void engine::CameraComponent::draw(Shader& shader, const glm::mat4& transform)
{

}
