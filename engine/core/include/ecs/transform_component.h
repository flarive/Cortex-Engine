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
		void draw() override;


		//unsigned int getTypeID() override
		//{
		//	return 1; // Unique ID for TransformComponent
		//}

	private:
		Transform m_transform{};       // local position/rotation/scale
		glm::mat4 m_worldTransform{};  // should be full parent * local
	};
}