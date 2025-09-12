#pragma once

#include "component.h"

#include "../cameras/camera.h"
//#include "../frustrum.h"
//#include "../bounding_volume.h"
//#include "../aabb.h"

namespace engine
{
	class CameraComponent : public ComponentBase<CameraComponent>
	{
	public:

		CameraComponent() = default;
		CameraComponent(std::shared_ptr<Camera> camera);
		~CameraComponent() = default;

		void init() override;
		void update(Transform& transform) override;
		void draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& transform) override;

		std::shared_ptr<Camera> getCamera()
		{
			return m_camera;
		}


		static unsigned int getStaticTypeID() {
			return 2;
		}

	private:

		std::shared_ptr<Camera> m_camera{};
		std::unique_ptr<AABB> m_boundingVolume{};

		engine::AABB generateAABB(const std::shared_ptr<Camera> camera);


		std::unique_ptr<AABB> getBoundingVolume() override;
	};
}