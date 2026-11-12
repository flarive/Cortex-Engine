#pragma once

#include "renderer.h"

namespace engine
{
	class VulkanRenderer : public Renderer
	{
	public:
		VulkanRenderer(GLFWwindow* window, RenderMethod method);
		virtual ~VulkanRenderer();

	protected:
		void loadShaders() override;
	};
}