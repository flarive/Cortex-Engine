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

	std::vector<std::shared_ptr<engine::Scene>>& getScenes() { return m_scenes; }

	bool shouldUnloadScene() const { return m_shouldUnloadScene; }
	void requestSceneUnload() { m_shouldUnloadScene = true; }

	void clearScenes();

	void addScene(std::shared_ptr<engine::Scene> scene);

	bool unloadCurrentScene(); // shouldn't be called directly, use requestSceneUnload instead
	void clean();

	private:
		std::vector<std::shared_ptr<engine::Scene>> m_scenes{};
		std::shared_ptr<engine::Scene> m_currentScene{};

		bool m_shouldUnloadScene{ false };
	};
}