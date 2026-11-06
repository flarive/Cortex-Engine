#pragma once

#include "renderer.h"
#include "../misc/skybox.h"

#include "../shaders/shader.h"

namespace engine
{
	class BlinnPhongRenderer final : public Renderer
	{
	public:
		// Main Blinn Phong shader
		std::unique_ptr<ShaderBase> blinnPhongShader{};
		std::unique_ptr<ShaderBase> blinnPhongShaderTessellation{};

		// Skybox shader
		std::unique_ptr<ShaderBase> skyboxReflectionShader{};
		std::unique_ptr<ShaderBase> backgroundShader{};
		
		BlinnPhongRenderer(GLFWwindow* window);
		~BlinnPhongRenderer() override;
		
		void setup(int width, int height, std::shared_ptr<Camera> camera, const std::vector<std::shared_ptr<Light>>& lights) override;
		void loop(int width, int height, std::shared_ptr<Camera> camera, std::function<void(std::unique_ptr<ShaderBase>&, std::unique_ptr<ShaderBase>&)> update, std::function<void()> updateUI) override;

		std::unique_ptr<engine::ShaderBase>& getShader() override;

		void setSkybox(const std::vector<std::string>& faces);

		void clean() override;

	private:
		void loadShaders() override;

		void initBackground();
		void renderBackground(const SceneSettings& settings);

		void setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount, unsigned int areaLightCount) override;


	protected:
		std::shared_ptr<Skybox> m_skybox{};

		unsigned int backgroundVBO{}, backgroundVAO{};
	};
}