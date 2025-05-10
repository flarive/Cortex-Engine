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
		
		// shaders accessible publicly

		// Shader that renders the color framebuffer to the screen
		Shader screenShader{};

		// Shader that renders a depth framebuffer for shadow maps
		Shader simpleDepthShader{};
		Shader debugDepthQuad{};

		Shader skyboxReflectShader{};
		Shader backgroundShader{};

		
		Renderer(GLFWwindow* window, const SceneSettings& settings);
		virtual ~Renderer() = default;

		virtual void setup(int width, int height, std::shared_ptr<Camera> camera, const std::vector<std::shared_ptr<engine::Light>>& lights) = 0;
		virtual void loop(int width, int height, std::shared_ptr<Camera> camera, std::function<void(Shader&)> update, std::function<void()> updateUI) = 0;

		void initColorFramebuffer(int width, int height);


		virtual void setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount) = 0;

	protected:
		GLFWwindow* m_window{};

		//std::shared_ptr<Camera> m_camera{};

		SceneSettings m_settings{};

		std::vector<std::shared_ptr<engine::Light>> m_lights{};

		// shadow maps texture size
		const unsigned int SHADOW_WIDTH{ 2048 }, SHADOW_HEIGHT{ 2048 };

		unsigned int rbo{}; // renderbuffer object

		

		// main framebuffer
		unsigned int colorFramebuffer{};

		// depth map framebuffer for shadow maps
		unsigned int depthMapFramebuffer{};

		// texture ID that holds main framebuffer rendering
		unsigned int textureColorBuffer{};

		// texture ID that holds depth map framebuffer
		unsigned int textureDepthMapBuffer{};
		

		// internal shaders (not accessible)
		// PBR
		Shader equirectangularToCubemapShader{};
		Shader irradianceShader{};
		Shader prefilterShader{};
		Shader brdfShader{};




		unsigned int irradianceMap{};
		unsigned int prefilterMap{};
		unsigned int brdfLUTTexture{};
		unsigned int envCubemap{};



		unsigned short m_spotLightCount{};
		unsigned short m_dirLightCount{};
		unsigned short m_pointLightCount{};


		void enableDepthTest(bool enable);
		void enableFaceCulling(bool enable);
		void enableAntiAliasing(bool enable);
		void enableGammaCorrection(bool enable);

		void initDepthMapFramebuffer();
		void computeDepthMapFramebuffer(Shader& shader, int width, int height, std::function<void(Shader&)> update, std::shared_ptr<Light> light);
		
		void computeColorFramebuffer();


		// renderCube() renders a 1x1 3D cube in NDC.
		// -------------------------------------------------
		unsigned int cubeVAO{};
		unsigned int cubeVBO{};
		void renderCube();


		// renderQuad() renders a 1x1 XY quad in NDC
		// -----------------------------------------
		unsigned int quadVAO{};
		unsigned int quadVBO{};
		void renderQuad();



		// renders (and builds at first invocation) a sphere
		// -------------------------------------------------
		unsigned int sphereVAO{};
		GLsizei indexCount{};
		void renderSphere();


	private:

	};
}