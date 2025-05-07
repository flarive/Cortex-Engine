#pragma once

#include "renderer.h"

namespace engine
{
	class PbrRenderer : public Renderer
	{
	public:

		PbrRenderer(GLFWwindow* window, const Camera& camera, std::vector<std::shared_ptr<engine::Light>> lights);

		void setup(const SceneSettings& settings, int width, int height) override;
		void loop(int width, int height) override;

	private:
		void loadShaders();
	};
}

