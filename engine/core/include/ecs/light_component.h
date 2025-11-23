#pragma once


#include "component.h"

#include "../lights/light.h"
#include "../frustrum.h"
#include "../bounding_volume.h"
#include "../aabb.h"


namespace engine
{
	class LightComponent final : public ComponentBase<LightComponent>
	{
	public:

		LightComponent() = default;
		LightComponent(std::shared_ptr<Light> light);
		~LightComponent() = default;

		void init(Transform& transform) override;
		void update(Transform& transform) override;

		void draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform, AABB* boundingVolume = nullptr) override;

		std::shared_ptr<Light> getLight()
		{
			return m_light;
		}


		static ComponentType getStaticTypeID() {
			return ComponentType::light;
		}

		std::string getName() override
		{
			return "Light";
		}

		engine::AABB* getBoundingVolume() override;

		ordered_map<std::string, std::variant<int, std::string, float, bool>> getPublicProperties() override;
		std::unordered_map<std::string, std::function<void(float)>> getPropertySetters() override;

	private:

		std::shared_ptr<Light> m_light{};
		std::unique_ptr<AABB> m_boundingVolume{};

		AABB generateBoundingVolume(const std::shared_ptr<Light> light);
	};
}