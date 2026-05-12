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
	void loadScenes(const std::vector<std::shared_ptr<engine::Scene>>& scenes);

	std::shared_ptr<engine::Scene> getCurrentScene();

	bool shouldUnloadScene() const { return m_shouldUnloadScene; }
	void requestSceneUnload() { m_shouldUnloadScene = true; }

	bool unloadCurrentScene(); // shouldn't be called directly, use requestSceneUnload instead
	void clean();

	private:
		std::vector<std::shared_ptr<engine::Scene>> m_scenes{};
		std::shared_ptr<engine::Scene> m_currentScene{};

		bool m_shouldUnloadScene{ false };
	};
}