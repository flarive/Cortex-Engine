#include "../../include/ecs/camera_component.h"

engine::CameraComponent::CameraComponent(std::shared_ptr<Camera> camera)
    : m_camera(camera)
{
	m_boundingVolume = std::make_unique<AABB>(generateBoundingVolume(camera));
}


void engine::CameraComponent::init(Transform& transform)
{
	m_camera->position = transform.getLocalPosition();
}

void engine::CameraComponent::update(Transform& transform)
{
	//m_camera->position = transform.getLocalPosition();
}

void engine::CameraComponent::draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& transform)
{

}

void engine::CameraComponent::draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation)
{

}

engine::AABB engine::CameraComponent::generateBoundingVolume(const std::shared_ptr<Camera> camera)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

	return engine::AABB(minAABB, maxAABB);
}

std::unique_ptr<engine::AABB> engine::CameraComponent::getBoundingVolume()
{
	return std::move(m_boundingVolume);
}
