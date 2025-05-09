#pragma once


#include "../app/scene_settings.h"

#include "../shader.h"
#include "../lights/light.h"
#include "../camera.h"
#include "../tools/file_system.h"

namespace engine
{
	/// <summary>
	/// Abstract class for renderers
	/// </summary>
	class Renderer
	{
	public:
		
		Renderer(GLFWwindow* window, const SceneSettings& settings);
		virtual ~Renderer() = default;

		virtual void setup(int width, int height, std::shared_ptr<Camera> camera, const std::vector<std::shared_ptr<engine::Light>>& lights) = 0;
		virtual void loop(int width, int height, std::shared_ptr<Camera> camera, std::function<void(Shader&)> update, std::function<void()> updateUI) = 0;

		void initColorFramebuffer(int width, int height);


		void setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount);

	protected:
		GLFWwindow* m_window{};

		//std::shared_ptr<Camera> m_camera{};

		SceneSettings m_settings{};

		std::vector<std::shared_ptr<engine::Light>> m_lights{};

		const unsigned int SHADOW_WIDTH{ 2048 }, SHADOW_HEIGHT{ 2048 };

		unsigned int rbo{}; // renderbuffer object

		unsigned int depthMapFramebuffer{};
		unsigned int colorFramebuffer{};

		unsigned int textureDepthMapBuffer{};
		unsigned int textureColorbuffer{};

		Shader screenShader{};
		Shader simpleDepthShader{};
		Shader debugDepthQuad{};

		Shader blinnPhongShader{};
		Shader pbrShader{};
		Shader skyboxReflectShader{};
		Shader backgroundShader{};

		// PBR
		Shader equirectangularToCubemapShader{};
		Shader irradianceShader{};
		Shader prefilterShader{};
		Shader brdfShader{};




		unsigned int irradianceMap{};
		unsigned int prefilterMap{};
		unsigned int brdfLUTTexture{};
		unsigned int envCubemap{};


		void enableDepthTest(bool enable);
		void enableFaceCulling(bool enable);
		void enableAntiAliasing(bool enable);
		void enableGammaCorrection(bool enable);

		void initDepthMapFramebuffer();
		void computeDepthMapFramebuffer(Shader& shader, int width, int height, std::function<void(Shader&)> update, std::shared_ptr<Light> light);
		
		void computeColorFramebuffer();


		// renderCube() renders a 1x1 3D cube in NDC.
		// -------------------------------------------------
		unsigned int cubeVAO = 0;
		unsigned int cubeVBO = 0;
		void renderCube();


		// renderQuad() renders a 1x1 XY quad in NDC
		// -----------------------------------------
		unsigned int quadVAO = 0;
		unsigned int quadVBO = 0;
		void renderQuad();



		// renders (and builds at first invocation) a sphere
		// -------------------------------------------------
		unsigned int sphereVAO = 0;
		GLsizei indexCount;
		void renderSphere();


	private:

		unsigned short m_spotLightCount{};
		unsigned short m_dirLightCount{};
		unsigned short m_pointLightCount{};
	};
}