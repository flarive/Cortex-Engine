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
		void update() override;
		void draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& transform) override;

		static unsigned int getStaticTypeID() {
			return 1; // Ensure this is the smallest TypeID
		}


	private:
		Transform m_transform{};       // local position/rotation/scale
		glm::mat4 m_worldTransform{};  // should be full parent * local
	};
}