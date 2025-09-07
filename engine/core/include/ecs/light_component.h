#pragma once


#include "component.h"

#include "../lights/point_light.h"
#include "../lights/directional_light.h"
#include "../lights/spot_light.h"

namespace engine
{
	class LightComponent : public ComponentBase<LightComponent>
	{
	public:

		LightComponent() = default;
		LightComponent(std::shared_ptr<Light> light);
		~LightComponent() = default;

		void init() override;
		void update() override;
		void draw() override;

		std::shared_ptr<Light> getLight()
		{
			return m_light;
		}


		static unsigned int getStaticTypeID() {
			return 4;
		}

	private:

		std::shared_ptr<Light> m_light{};

	};
}