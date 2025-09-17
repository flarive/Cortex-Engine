#pragma once


#include "component.h"

#include "../lights/light.h"
#include "../frustrum.h"
#include "../bounding_volume.h"
#include "../aabb.h"


namespace engine
{
	class LightComponent : public ComponentBase<LightComponent>
	{
	public:

		LightComponent() = default;
		LightComponent(std::shared_ptr<Light> light);
		~LightComponent() = default;

		void init(Transform& transform) override;
		void update(Transform& transform) override;

		void draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& transform) override;
		void draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation) override;


		std::shared_ptr<Light> getLight()
		{
			return m_light;
		}


		static ComponentType getStaticTypeID() {
			return ComponentType::light;
		}

		std::unique_ptr<AABB> getBoundingVolume() override;

	private:

		std::shared_ptr<Light> m_light{};
		std::unique_ptr<AABB> m_boundingVolume{};

		AABB generateBoundingVolume(const std::shared_ptr<Light> light);
	};
}