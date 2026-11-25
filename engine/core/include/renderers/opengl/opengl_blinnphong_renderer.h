#pragma once

#include "../opengl_renderer.h"
#include "../../misc/skybox.h"

#include "../../shaders/shader.h"

namespace engine
{
	class OpenGLBlinnPhongRenderer final : public OpenGLRenderer
	{
	public:
		// Main Blinn Phong shader
		std::unique_ptr<Shader> blinnPhongShader{};
		std::unique_ptr<Shader> blinnPhongShaderTessellation{};

		// Skybox shader
		std::unique_ptr<Shader> skyboxReflectionShader{};
		std::unique_ptr<Shader> backgroundShader{};
		
		OpenGLBlinnPhongRenderer(GLFWwindow* window);
		~OpenGLBlinnPhongRenderer() override;
		
		void setup(int width, int height, const std::shared_ptr<Camera>& camera, const std::vector<std::shared_ptr<Light>>& lights) override;
		void loop(int width, int height, std::shared_ptr<Camera>& camera, std::function<void(std::unique_ptr<Shader>&, std::unique_ptr<Shader>&)> update, std::function<void()> updateUI, const std::string& hash) override;

		std::unique_ptr<engine::Shader>& getShader() override;

		void setSkybox(const std::vector<std::string>& faces);

		

		void clean() override;

	private:
		// init
		void initRenderStates(const SceneSettings& settings) override;
		void loadShaders();
		void initCommonTextures();
		void initFramebuffers(const SceneSettings& settings, int width, int height) override;
		void setupShaderBindings(const SceneSettings& settings) override;
		void setupScreenShaderBindings() override;
		void setupBackgroundShaderBindings() override;
		void initBackground();


		// loop
		void beginFrame(const SceneSettings& settings, int width, int height) override;
		void renderDebug(const engine::FrameData& frameData, const engine::SceneSettings& settings);
		void renderSkybox(const engine::FrameData& frameData, const engine::SceneSettings& settings);
		void setupBlinnPhongPass(const engine::FrameData& frameData, const engine::SceneSettings& settings);
		void renderScene(std::function<void(std::unique_ptr<Shader>&, std::unique_ptr<Shader>&)> update);
		void renderShadowPass(int width, int height, const engine::SceneSettings& settings, std::function<void(std::unique_ptr<Shader>&, std::unique_ptr<Shader>&)> update);
		void resolveMSAA(int width, int height);
		void renderFramebuffer(const engine::SceneSettings& settings);
		void renderUI(std::function<void()> updateUI);
		void endFrame() override;



		void renderBackground(const SceneSettings& settings);

		void setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount, unsigned int areaLightCount) override;


	protected:
		std::shared_ptr<Skybox> m_skybox{};

		unsigned int backgroundVBO{}, backgroundVAO{};
	};
}