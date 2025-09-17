#pragma once

#include "component.h"

#include "../cameras/camera.h"

namespace engine
{
	class CameraComponent : public ComponentBase<CameraComponent>
	{
	public:

		CameraComponent() = default;
		CameraComponent(std::shared_ptr<Camera> camera);
		~CameraComponent() = default;

		void init(Transform& transform) override;
		void update(Transform& transform) override;

		void draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& transform) override;
		void draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation) override;

		std::shared_ptr<Camera> getCamera()
		{
			return m_camera;
		}


		static ComponentType getStaticTypeID() {
			return ComponentType::camera;
		}

		std::unique_ptr<AABB> getBoundingVolume() override;

	private:

		std::shared_ptr<Camera> m_camera{};
		std::unique_ptr<AABB> m_boundingVolume{};

		AABB generateBoundingVolume(const std::shared_ptr<Camera> camera);
	};
}