#pragma once


#include "component.h"

#include "../cameras/fly_camera.h"
#include "../cameras/fps_camera.h"
#include "../cameras/orbit_camera.h"

namespace engine
{
	class CameraComponent : public ComponentBase<CameraComponent>
	{
	public:

		CameraComponent() = default;
		CameraComponent(std::shared_ptr<Camera> camera);
		~CameraComponent() = default;

		void init() override;
		void update() override;
		void draw() override;

		std::shared_ptr<Camera> getCamera()
		{
			return m_camera;
		}


		static unsigned int getStaticTypeID() {
			return 2;
		}

	private:

		std::shared_ptr<Camera> m_camera{};
		
	};
}