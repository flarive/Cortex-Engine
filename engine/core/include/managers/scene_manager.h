#pragma once

#include "../common_defines.h"

#include "../app/scene.h"

namespace engine
{
	class SceneManager final
	{
	public:
		SceneManager() = default;
		~SceneManager() = default;

		void loadScene(std::shared_ptr<Scene> scene);
		std::shared_ptr<Scene> getCurrentScene();

	private:
		std::shared_ptr<Scene> m_currentScene;
	};
}