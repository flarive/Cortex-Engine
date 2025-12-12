#pragma once

#include "component.h"

#include "../cameras/camera.h"

namespace engine
{
	class CameraComponent final : public ComponentBase<CameraComponent>
	{
	public:

		CameraComponent() = default;
		CameraComponent(std::shared_ptr<Camera> camera);
		~CameraComponent() = default;

		void init(Transform& transform) override;
		void update(float deltaTime, Transform& transform) override;

		void draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform, AABB* boundingVolume = nullptr) override;

		std::shared_ptr<Camera> getCamera()
		{
			return m_camera;
		}


		static ComponentType getStaticTypeID() {
			return ComponentType::camera;
		}

		std::string getName() override
		{
			return "Camera";
		}

		AABB* getBoundingVolume() override;

		ordered_map<std::string, EditorProperty> getPublicProperties() override;
		std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() override;

		void setProperty(const std::string& key, EditorPropertyValue value) override;

	private:

		std::shared_ptr<Camera> m_camera{};
		std::unique_ptr<AABB> m_boundingVolume{};

		std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> m_propertySetters{};

		AABB generateBoundingVolume(const std::shared_ptr<Camera> camera);
	};
}