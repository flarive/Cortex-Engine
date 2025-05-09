#pragma once

#include "renderer.h"

namespace engine
{
	class PbrRenderer : public Renderer
	{
	public:

		PbrRenderer(GLFWwindow* window, const SceneSettings& settings, const Camera& camera);

		void setup(int width, int height, std::vector<std::shared_ptr<engine::Light>> lights) override;
		void loop(int width, int height, std::function<void(Shader&)> update, std::function<void()> updateUI) override;

	private:
		void loadShaders();
	};
}

