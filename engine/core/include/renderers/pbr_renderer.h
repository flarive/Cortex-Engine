#pragma once

#include "renderer.h"

#include "../shaders/shader.h"

namespace engine
{
	class PbrRenderer final : public Renderer
	{
	public:
		// Main PBR shader
		std::unique_ptr<ShaderBase> pbrShader{};
		std::unique_ptr<ShaderBase> pbrShaderTessellation{};

		// PBR environmentMap shader (for HDR skybox)
		std::unique_ptr<ShaderBase> backgroundShader{};

		PbrRenderer(GLFWwindow* window);
		~PbrRenderer() override;

		void setup(int width, int height, std::shared_ptr<Camera> camera, const std::vector<std::shared_ptr<engine::Light>>& lights) override;
		void loop(int width, int height, std::shared_ptr<Camera> camera, std::function<void(std::unique_ptr<ShaderBase>&, std::unique_ptr<ShaderBase>&)> update, std::function<void()> updateUI) override;

		std::unique_ptr<ShaderBase>& getShader() override;

		void clean() override;

	private:

		unsigned int captureFBO{};
		unsigned int captureRBO{};

		void loadShaders() override;

		void setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount, unsigned int areaLightCount) override;

		void initBloomBuffers(int width, int height);

		void extractBloomBrightPass(float bloomThreshold);
		void blurBloomTexture(ubyte bloomBlurPasses);
	};
}