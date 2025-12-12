#pragma once


#include "component.h"

#include "../transform.h"

namespace engine
{
	class TransformComponent final : public ComponentBase<TransformComponent>
	{
	public:
		
		TransformComponent() = default;
		TransformComponent(const Transform& transform);
		~TransformComponent() = default;

		void init(Transform& transform) override;
		void update(float deltaTime, Transform& transform) override;

		void draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform, AABB* boundingVolume = nullptr) override;

		static ComponentType getStaticTypeID()
		{
			return ComponentType::transform;
		}

		std::string getName() override
		{
			return "Transform";
		}

		Transform getTransform() { return m_transform; }
		glm::mat4 getWorldTransformMatrix() { return m_worldTransform; }


		void setTransform(const Transform& transform) { m_transform = transform; }
		void setWorldTransformMatrix(const glm::mat4& worldTransform)
		{
			m_worldTransform = worldTransform;
		}


		ordered_map<std::string, EditorProperty> getPublicProperties() override;
		std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() override;

		void setProperty(const std::string& key, EditorPropertyValue value) override;

	private:
		Transform m_transform{};       // local position/rotation/scale
		glm::mat4 m_worldTransform{};  // should be full parent * local

		engine::AABB* getBoundingVolume() override;
	};
}