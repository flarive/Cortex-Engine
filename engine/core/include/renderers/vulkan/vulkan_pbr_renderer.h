#pragma once

#include "../vulkan_renderer.h"

#include "../../shaders/shader.h"

#include <array>

namespace engine
{
	class VulkanPbrRenderer final : public VulkanRenderer
	{
	public:
		// Main PBR shader
		std::unique_ptr<Shader> pbrShader{};
		std::unique_ptr<Shader> pbrShaderTessellation{};

		// PBR environmentMap shader (for HDR skybox)
		std::unique_ptr<Shader> backgroundShader{};

		VulkanPbrRenderer(GLFWwindow* window);
		~VulkanPbrRenderer() override;

		void setup(int width, int height, const std::shared_ptr<Camera>& camera, const std::vector<std::shared_ptr<engine::Light>>& lights) override;
		void loop(int width, int height, std::shared_ptr<Camera>& camera, std::function<void(std::unique_ptr<Shader>&, std::unique_ptr<Shader>&)> update, std::function<void()> updateUI, const std::string& hash) override;

		std::unique_ptr<Shader>& getShader() override;

		void clean() override;

	private:

		unsigned int captureFBO{};
		unsigned int captureRBO{};

		glm::mat4 m_captureProjection{};
		std::array<glm::mat4, 6> m_captureViews{};


		// init
		void initRenderStates(const SceneSettings& settings) override;
		void loadShaders() override;

		void initCommonTextures();

		void initFramebuffers(const SceneSettings& settings, int width, int height) override;
		void initBloomBuffers(int width, int height);

		void setupShaderBindings(const SceneSettings& settings) override;
		void setupScreenShaderBindings() override;
		void setupBackgroundShaderBindings() override;

		void initCaptureMatrices();
		void initCaptureResources(int visze);

		void generateEnvironmentMap(const SceneSettings& settings, int vsize);
		void generateIrradianceMap();
		void generatePrefilterMap();
		void generateBRDFLUT(int vsize);

		void restoreScreenViewport();



		// loop
		void beginFrame(const SceneSettings& settings, int width, int height) override;


		void renderDebug(const FrameData& frameData, const SceneSettings& settings);
		void renderSkybox(const FrameData& frameData, const SceneSettings& settings);
		void setupPBRPass(const engine::FrameData& frameData, const engine::SceneSettings& settings);
		void renderScene(std::function<void(std::unique_ptr<Shader>&, std::unique_ptr<Shader>&)> update);
		void renderShadowPass(int width, int height, const engine::SceneSettings& settings, std::function<void(std::unique_ptr<Shader>&, std::unique_ptr<Shader>&)> update);
		void resolveMSAA(int width, int height);
		void renderBloom(const engine::SceneSettings& settings);
		void renderFramebuffer(int width, int height, const engine::SceneSettings& settings);
		void renderUI(std::function<void()> updateUI);

		void endFrame() override;


		void setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount, unsigned int areaLightCount) override;



		void extractBloomBrightPass(float bloomThreshold);
		void blurBloomTexture(ubyte bloomBlurPasses);
	};
}