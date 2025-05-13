#pragma once

#include "renderer.h"

#include "../shader.h"

namespace engine
{
	class PbrRenderer : public Renderer
	{
	public:
		// Main PBR shader
		Shader pbrShader{};

		// PBR environmentMap shader (for HDR skybox)
		Shader backgroundShader{};
		
		PbrRenderer(GLFWwindow* window, const SceneSettings& settings);

		void setup(int width, int height, std::shared_ptr<Camera> camera, const std::vector<std::shared_ptr<engine::Light>>& lights) override;
		void loop(int width, int height, std::shared_ptr<Camera> camera, std::function<void(Shader&)> update, std::function<void()> updateUI) override;

		Shader& getShader() override;

	private:
		void loadShaders() override;

		void setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount) override;
	};
}

