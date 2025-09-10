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
		void draw(Shader& shader, const glm::mat4& transform) override;

		std::shared_ptr<Light> getLight()
		{
			return m_light;
		}


		static unsigned int getStaticTypeID() {
			return 5;
		}

	private:

		std::shared_ptr<Light> m_light{};

	};
}