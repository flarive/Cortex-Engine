#include "../../include/managers/scene_manager.h"

#include "../../include/app/scene.h"

void engine::SceneManager::loadScene(std::shared_ptr<engine::Scene> scene)
{
	if (m_currentScene) {
		m_currentScene->clean();
	}
	m_currentScene = std::move(scene);
}

void engine::SceneManager::loadScenes(const std::vector<std::shared_ptr<engine::Scene>>& scenes)
{
	m_scenes = scenes;

	if (!scenes.empty())
		m_currentScene = std::move(m_scenes[0]);
}

std::shared_ptr<engine::Scene> engine::SceneManager::getCurrentScene()
{
	return m_currentScene;
}

bool engine::SceneManager::unloadCurrentScene()
{
	if (m_currentScene)
	{
		m_currentScene->exit();
		m_currentScene.reset(); // force destroying the scene and calling destructor

		return true;
	}

	return false;
}

void engine::SceneManager::clean()
{

}
