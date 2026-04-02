#pragma once

#include "../app/scene_settings.h"

#include "../shader.h"
#include "../lights/light.h"
#include "../lights/point_light.h"
#include "../cameras/camera.h"

#include "../debug/debug_plane_grid.h"

#include "../misc/ltc_matrix.h"


namespace engine
{
	/// <summary>
	/// Abstract class for renderers
	/// In the learnopengl.com tutorials and in most OpenGL contexts by convention:

	// + Y points up(toward the top of the screen).
	// - Y points down(toward the bottom of the screen).

	// This is the standard coordinate system used in OpenGL and many 3D graphics applications :

	// + X : Right
	// - X : Left
	// + Y : Up
	// - Y : Down
	// + Z : Out of the screen(toward the viewer)
	// - Z : Into the screen(away from the viewer)
	/// </summary>
	class Renderer
	{
	public:
		// shaders accessible publicly

		// Shader that renders the color framebuffer to the screen
		Shader screenShader{};

		// Shader that renders a depth framebuffer for shadow maps
		Shader directionalDepthMapShader{};
		Shader directionalDepthMapTessellationShader{};

		Shader pointDepthMapShader{};
		Shader pointDepthMapTessellationShader{};
		
		Shader depthMapToQuadShader{}; // for depth map texture debugging purposes
		Shader cubeFaceDebugShader{}; // for depth map cube map textures debugging purposes

		
		

		
		Renderer(GLFWwindow* window);
		virtual ~Renderer() = default;

		virtual void setup(int width, int height, std::shared_ptr<Camera> camera, const std::vector<std::shared_ptr<Light>>& lights) = 0;
		virtual void loop(int width, int height, std::shared_ptr<Camera> camera, std::function<void(Shader&, Shader&)> update, std::function<void()> updateUI) = 0;

		void initColorFramebuffer(int width, int height);
		void initColorFramebufferMSAA(int width, int height);
		void initHDRColorFramebufferMSAA(int width, int height);

		virtual void setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount, unsigned int areaLightCount) = 0;

		virtual Shader& getShader() = 0;

		virtual void clean() = 0;

	protected:
		GLFWwindow* m_window{};

		std::shared_ptr<Camera> m_camera{};

		std::vector<std::shared_ptr<Light>> m_lights{};

		// renderbuffer object to render the main framebuffer
		unsigned int rbo{};

		// main framebuffer
		unsigned int colorFramebuffer{};

		// depth map framebuffer for shadow maps
		unsigned int depthMapFramebuffer{};

		// texture ID that holds main framebuffer rendering
		unsigned int textureColorBuffer{};

		// texture ID that holds depth map framebuffer
		unsigned int textureDepthMapBuffer{};

		
		GLint m_prevPolyModes[2]{};

		// internal shaders (not accessible)
		// PBR
		Shader equirectangularToCubemapShader{};
		Shader irradianceShader{};
		Shader prefilterShader{};
		Shader brdfShader{};

		Shader outlineColorShader{};

		

		// HDR
		//unsigned int resolvedHDRTex{};
		unsigned int resolveFBO{};
		//unsigned int resolveFBOTexture{};


		unsigned int irradianceMap{};
		unsigned int prefilterMap{};
		unsigned int brdfLUTTexture{};
		unsigned int envCubemap{};

		// for area lights
		unsigned int LTC1Map{};
		unsigned int LTC2Map{};



		unsigned short m_spotLightCount{};
		unsigned short m_dirLightCount{};
		unsigned short m_pointLightCount{};
		unsigned short m_areaLightCount{};

		DebugPlaneGrid m_debugPlaneGrid{};

		virtual void loadShaders() = 0;
		
		


		void enableDepthTest(bool enable);
		void enableStencilTest(bool enable);
		void enableFaceCulling(bool enable);
		

		void initDepthMapFramebuffer(GLsizei shadowSize);
		void computeDepthMapFramebuffer(GLsizei width, GLsizei height, bool enableShadows, GLsizei shadowMapsTextureSize, Shader& shader, Shader& shaderTessellation, std::function<void(Shader&, Shader&)> update);




		void initDebugPlaneGrid();
		void renderDebugPlaneGrid(const glm::mat4& projection, const glm::mat4& view);

		void computeColorFramebuffer(const SceneSettings& settings);
		void computeHDRColorFramebuffer(int width, int height, const SceneSettings& settings);

		void updateEditorPropertySettings();


		// renderCube() renders a 1x1 3D cube in NDC.
		// -------------------------------------------------
		unsigned int m_cubeVAO{};
		unsigned int m_cubeVBO{};
		void renderCube();


		// renderQuad() renders a 1x1 XY quad in NDC
		// -----------------------------------------
		unsigned int m_quadVAO{};
		unsigned int m_quadVBO{};
		void renderQuad();

		// renders (and builds at first invocation) a sphere
		// -------------------------------------------------
		unsigned int m_sphereVAO{};
		GLsizei m_indexCount{};
		void renderSphere();


		// base
		unsigned int U_BG_ENV = 5;
		unsigned int U_SHADOW_MAP = 1; // pspot lights & dir lights
		unsigned int U_SHADOW_MAP_CUBE = 2; // point lights
		unsigned int U_LTC1 = 3; // area lights
		unsigned int U_LTC2 = 4; // area lights


		// BlinnPhong
		// Renderer-reserved units (example, but validate against GL_MAX_TEXTURE_IMAGE_UNITS):



		// PBR
		// Renderer-reserved units (example, but validate against GL_MAX_TEXTURE_IMAGE_UNITS):
		
		unsigned int U_IRR = 7;
		unsigned int U_PREF = 8;
		unsigned int U_BRDF = 9;



	private:
		void testHDR(int width, int height);

		

		void initSpotLightDepthMapFramebuffer(GLsizei shadowSize); // for point light
		void initPointLightDepthMapFramebuffer(GLsizei shadowSize); // for omni light

		void computeSpotLightDepthMapFramebuffer(Shader& shader, Shader& shaderTessellation, GLsizei width, GLsizei height, bool enableShadows, GLsizei shadowSize, std::function<void(Shader&, Shader&)> update, std::shared_ptr<Light> light);
		void computePointLightDepthMapFramebuffer(Shader& shader, Shader& shaderTessellation, GLsizei width, GLsizei height, bool enableShadows, GLsizei shadowSize, std::function<void(Shader&, Shader&)> update, std::shared_ptr<engine::Light> light);
	};
}