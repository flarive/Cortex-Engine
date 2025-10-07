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

}

void engine::CameraComponent::draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform, AABB* boundingVolume)
{

}

engine::AABB engine::CameraComponent::generateBoundingVolume(const std::shared_ptr<Camera> camera)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

	return engine::AABB(minAABB, maxAABB);
}

engine::AABB* engine::CameraComponent::getBoundingVolume()
{
	return m_boundingVolume.get();
}

std::vector<engine::KeyValuePair> engine::CameraComponent::getPublicProperties()
{
	return std::vector<engine::KeyValuePair>{};
}
