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
	};
}