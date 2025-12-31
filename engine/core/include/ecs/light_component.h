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
		void update(float deltaTime, Transform& transform) override;

		void draw(const glm::mat4& projection, const glm::mat4& view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform, AABB* boundingVolume = nullptr) override;

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

		ordered_map<std::string, EditorProperty> getPublicProperties() override;
		std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() override;

		void setProperty(const std::string& key, EditorPropertyValue value) override;

	private:

		std::shared_ptr<Light> m_light{};
		std::unique_ptr<AABB> m_boundingVolume{};

		std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> m_propertySetters{};

		AABB generateBoundingVolume(const std::shared_ptr<Light> light);
	};
}