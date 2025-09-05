#pragma once


#include "component.h"

#include "../transform.h"

namespace engine
{
	class CameraComponent : public ComponentBase<CameraComponent>
	{
	public:

		CameraComponent() = default;
		~CameraComponent() = default;

		void init() override;
		void update() override;
		void draw() override;


		//unsigned int getTypeID() override
		//{
		//	return 2; // Unique ID for TransformComponent
		//}

	private:
		
	};
}