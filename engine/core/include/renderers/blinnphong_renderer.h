#pragma once

#include "renderer.h"

namespace engine
{
	class BlinnPhongRenderer : public Renderer
	{
	public:
		BlinnPhongRenderer(GLFWwindow* window, const Camera& camera, std::vector<std::shared_ptr<engine::Light>> lights);
		
		void setup(const SceneSettings& settings, int width, int height) override;
		void loop(int width, int height) override;

	private:
		void loadShaders();
	};
}

