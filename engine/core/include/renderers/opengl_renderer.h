#pragma once

#include "renderer.h"

namespace engine
{
	class OpenGLRenderer : public Renderer
	{
	public:
		OpenGLRenderer(GLFWwindow* window, RenderMethod method);
		virtual ~OpenGLRenderer();

	protected:
		void loadShaders() override;

		std::string_view getUniformSamplerName(TextureSlot type) const;
		bool bind(std::unique_ptr<Shader>& shader, std::vector<std::shared_ptr<Texture>>& textures, int baseUnit, bool resetHasTextures) const;
		void unbind(std::vector<std::shared_ptr<Texture>>& textures, int baseUnit) const;
	};
}