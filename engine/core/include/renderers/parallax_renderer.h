#pragma once

#include "renderer.h"
#include "../misc/skybox.h"

#include "../shader.h"

namespace engine
{
	class ParallaxRenderer final : public Renderer
	{
	public:
		Shader parallaxShader{};

		ParallaxRenderer(GLFWwindow* window);

		void setup(int width, int height, std::shared_ptr<Camera> camera, const std::vector<std::shared_ptr<Light>>& lights) override;
		void loop(int width, int height, std::shared_ptr<Camera> camera, std::function<void(Shader&, Shader&)> update, std::function<void()> updateUI) override;

		Shader& getShader() override;

		

		void clean() override;

	private:
		void loadShaders() override;

		
		void renderBackground(const SceneSettings& settings);

		void setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount, unsigned int areaLightCount) override;

	};
}