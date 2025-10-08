#pragma once

#include "renderer.h"
#include "../primitives/skybox.h"

#include "../shader.h"

namespace engine
{
	class BlinnPhongRenderer final : public Renderer
	{
	public:
		// Main Blinn Phong shader
		Shader blinnPhongShader{};

		// Skybox shader
		Shader skyboxShader{};
		
		BlinnPhongRenderer(GLFWwindow* window);
		
		void setup(int width, int height, std::shared_ptr<Camera> camera, const std::vector<std::shared_ptr<Light>>& lights) override;
		void loop(int width, int height, std::shared_ptr<Camera> camera, std::function<void(Shader&)> update, std::function<void()> updateUI) override;

		Shader& getShader() override;

		void setSkybox(const std::vector<std::string>& faces);

		void clean() override;

	private:
		void loadShaders() override;

		void setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount, unsigned int areaLightCount);


	protected:
		std::shared_ptr<Skybox> m_skybox{};
	};
}