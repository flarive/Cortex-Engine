#pragma once

#include "../opengl_renderer.h"
#include "../../misc/skybox.h"

#include "../../shaders/shader.h"

namespace engine
{

	struct PhongCameraUBO final
	{
		glm::mat4 view;
		glm::mat4 projection;
	};

	struct PhongModelUBO final
	{
		glm::mat4 model;
	};

	struct alignas(16) PhongUBO final
	{
		glm::vec4 lightPos;
		glm::vec4 viewPos;
		glm::vec4 lightColor;
		glm::vec4 objectColor;

		uint32_t hasDiffuseTexture;
	};

	class OpenGLPhongRenderer final : public OpenGLRenderer
	{
	public:
		// Main Phong shader
		std::unique_ptr<Shader> phongShader{};

	
		OpenGLPhongRenderer(GLFWwindow* window);
		~OpenGLPhongRenderer() override;
		
		void setup(int width, int height, const std::shared_ptr<Camera>& camera, const std::vector<std::shared_ptr<Light>>& lights) override;
		void loop(int width, int height, std::shared_ptr<Camera>& camera, std::function<void(std::unique_ptr<Shader>&, std::unique_ptr<Shader>&)> update, std::function<void()> updateUI) override;

		std::unique_ptr<Shader>& getShader() override;

		void clean() override;

	private:
		
		GLint sampleTextureID{};
		
		
		// init
		void initRenderStates(const SceneSettings& settings) override;
		void loadShaders() override;
		void initCommonTextures();
		void initFramebuffers(const SceneSettings& settings, int width, int height) override;
		void setupShaderBindings(const SceneSettings& settings) override;
		void setupScreenShaderBindings() override;
		void setupBackgroundShaderBindings() override;

		// loop
		void beginFrame(const SceneSettings& settings, int width, int height) override;
		void setupPhongPass(const engine::FrameData& frameData, const engine::SceneSettings& settings);
		void renderScene(std::function<void(std::unique_ptr<Shader>&, std::unique_ptr<Shader>&)> update);
		void resolveMSAA(int width, int height);
		void renderUI(std::function<void()> updateUI);
		void endFrame() override;

		void setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount, unsigned int areaLightCount) override;
	};
}