#pragma once


#include "component.h"

#include "../lights/light.h"

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
		void draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& transform) override;

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