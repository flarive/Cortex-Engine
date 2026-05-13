#include "../../include/managers/scene_manager.h"

#include "../../include/app/scene.h"

#include <algorithm>

void engine::SceneManager::addScene(std::shared_ptr<engine::Scene> scene)
{
	bool empty = m_scenes.empty();
	
	m_scenes.emplace_back(std::move(scene));

	if (empty)
	{
		// auto select the first one as current scene
		m_currentScene = m_scenes[0];
	}
}

std::shared_ptr<engine::Scene> engine::SceneManager::getCurrentScene()
{
	return m_currentScene;
}

std::shared_ptr<engine::Scene> engine::SceneManager::setCurrentScene(unsigned int sceneIndex)
{
	if (m_scenes.size() > sceneIndex)
	{
		if (m_currentScene)
		{
			unloadCurrentScene();
		}

		m_currentScene = m_scenes.at(sceneIndex);
		return m_currentScene;
	}

	return nullptr;
}

bool engine::SceneManager::setCurrentScene(const std::string& sceneName)
{
	if (m_scenes.empty())
		return false;

	if (m_currentScene)
	{
		unloadCurrentScene();
	}

	auto it = std::find_if(m_scenes.begin(), m_scenes.end(),
		[&](const std::shared_ptr<Scene>& obj) {
			return obj && obj->getName() == sceneName;
		});

	if (it != m_scenes.end())
	{
		m_currentScene = *it;
		return true;
	}

	return false;
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
