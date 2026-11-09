#pragma once

#include "../renderer.h"
#include "../../misc/skybox.h"

#include "../../shaders/shader.h"

namespace engine
{
	class OpenGLPhongRenderer final : public Renderer
	{
	public:
		// Main Phong shader
		std::unique_ptr<ShaderBase> phongShader{};

	
		OpenGLPhongRenderer(GLFWwindow* window);
		~OpenGLPhongRenderer() override;
		
		void setup(int width, int height, std::shared_ptr<Camera>& camera, const std::vector<std::shared_ptr<Light>>& lights) override;
		void loop(int width, int height, std::shared_ptr<Camera>& camera, std::function<void(std::unique_ptr<ShaderBase>&, std::unique_ptr<ShaderBase>&)> update, std::function<void()> updateUI) override;

		std::unique_ptr<ShaderBase>& getShader() override;

		void clean() override;

	private:
		// init
		void initRenderStates(const SceneSettings& settings) override;
		void loadShaders() override;
		void initFramebuffers(const SceneSettings& settings, int width, int height) override;
		void setupShaderBindings(const SceneSettings& settings) override;
		void setupScreenShaderBindings() override;
		void setupBackgroundShaderBindings() override;

		// loop
		void beginFrame(const SceneSettings& settings, int width, int height) override;
		void endFrame() override;

		void setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount, unsigned int areaLightCount) override;
	};
}