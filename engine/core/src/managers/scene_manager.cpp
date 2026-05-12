#include "../../include/managers/scene_manager.h"

#include "../../include/app/scene.h"

void engine::SceneManager::loadScene(std::shared_ptr<engine::Scene> scene)
{
	if (m_currentScene) {
		m_currentScene->clean();
	}
	m_currentScene = std::move(scene);
}

void engine::SceneManager::addScene(std::shared_ptr<engine::Scene> scene)
{
	m_scenes.emplace_back(std::move(scene));

	m_currentScene = m_scenes[0];
}

std::shared_ptr<engine::Scene> engine::SceneManager::getCurrentScene()
{
	return m_currentScene;
}

void engine::SceneManager::clearScenes()
{
	m_scenes.clear();
}

bool engine::SceneManager::unloadCurrentScene()
{
	if (m_currentScene)
	{
		// Remove the shared_ptr from the vector
		m_scenes.erase(std::remove(m_scenes.begin(), m_scenes.end(), m_currentScene), m_scenes.end());

		// Destroy scene and call destructor
		m_currentScene->exit();
		m_currentScene.reset(); // force destroying the scene and calling destructor

		return true;
	}

	return false;
}

void engine::SceneManager::clean()
{

}
