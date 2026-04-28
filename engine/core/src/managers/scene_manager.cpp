#include "../../include/managers/scene_manager.h"

void engine::SceneManager::loadScene(std::shared_ptr<engine::Scene> scene)
{
	if (m_currentScene) {
		m_currentScene->clean();
	}
	m_currentScene = std::move(scene);
	m_currentScene->init();
}

std::shared_ptr<engine::Scene> engine::SceneManager::getCurrentScene()
{
	return m_currentScene;
}
