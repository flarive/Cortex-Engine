#pragma once

#include "../common_defines.h"

namespace engine
{
	class Scene;
	
	class SceneManager final
	{
	public:
		SceneManager() = default;
		~SceneManager() = default;

	void loadScene(std::shared_ptr<engine::Scene> scene);
	std::shared_ptr<engine::Scene> getCurrentScene();
	bool unloadCurrentScene();

	private:
		std::shared_ptr<engine::Scene> m_currentScene{};
	};
}