#pragma once


#include "component.h"

#include "../transform.h"

namespace engine
{
	class TransformComponent : public ComponentBase<TransformComponent>
	{
	public:
		
		TransformComponent() = default;
		TransformComponent(const Transform& transform);
		~TransformComponent() = default;

		void init() override;
		void update(Transform& transform) override;
		void draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& transform) override;

		static unsigned int getStaticTypeID() {
			return 1; // Ensure this is the smallest TypeID
		}

		Transform getTransform() { return m_transform; }
		glm::mat4 getWorldTransformMatrix() { return m_worldTransform; }


		void setTransform(const Transform& transform) { m_transform = transform; }
		void setWorldTransformMatrix(const glm::mat4& worldTransform) { m_worldTransform = worldTransform; }


	private:
		Transform m_transform{};       // local position/rotation/scale
		glm::mat4 m_worldTransform{};  // should be full parent * local

		std::unique_ptr<AABB> getBoundingVolume() override;
	};
}