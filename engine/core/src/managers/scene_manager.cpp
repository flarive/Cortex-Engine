#include "../../include/managers/scene_manager.h"

#include "../../include/app/scene.h"

void engine::SceneManager::loadScene(std::shared_ptr<engine::Scene> scene)
{
	if (m_currentScene) {
		m_currentScene->clean();
	}
	m_currentScene = std::move(scene);
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
	}

	return false;
}

void engine::SceneManager::clean()
{
	if (m_currentScene) {
		//m_currentScene->clean();
		//m_currentScene.reset();
	}
}
