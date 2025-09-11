#include "../../include/ecs/camera_component.h"
//
//#include "../../include/cameras/fly_camera.h"
//#include "../../include/cameras/fps_camera.h"
//#include "../../include/cameras/orbit_camera.h"

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

void engine::CameraComponent::draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& transform)
{

}
