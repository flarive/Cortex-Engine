#pragma once

#include "renderer.h"
#include "../misc/skybox.h"

#include "../shaders/shader.h"

namespace engine
{
	class PhongRenderer final : public Renderer
	{
	public:
		// Main Phong shader
		std::unique_ptr<ShaderBase> phongShader{};

	
		PhongRenderer(GLFWwindow* window);
		~PhongRenderer() override;
		
		void setup(int width, int height, std::shared_ptr<Camera> camera, const std::vector<std::shared_ptr<Light>>& lights) override;
		void loop(int width, int height, std::shared_ptr<Camera> camera, std::function<void(std::unique_ptr<ShaderBase>&, std::unique_ptr<ShaderBase>&)> update, std::function<void()> updateUI) override;

		std::unique_ptr<ShaderBase>& getShader() override;

		void clean() override;

	private:
		void loadShaders() override;

		void setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount, unsigned int areaLightCount) override;
	};
}