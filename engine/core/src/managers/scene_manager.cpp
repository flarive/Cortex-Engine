#include "../../include/managers/scene_manager.h"

#include "../../include/app/scene.h"

#include "../../include/managers/log_manager.h"
#include "../../include/singleton.h"


#include <algorithm>


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
			unloadCurrentScene(); // Destroy the current scene
		}

		// Recreate the scene using the factory
		m_currentScene = m_scenes.at(sceneIndex).factory();
		engine::Singleton::reset(m_currentScene->getSceneSettings());
		return m_currentScene;
	}
	return nullptr;
}

bool engine::SceneManager::setCurrentScene(const std::string& sceneName)
{
	if (m_scenes.empty())
		return false;

	// Unload and destroy the current scene
	if (m_currentScene)
	{
		unloadCurrentScene();
	}

	// Find the scene in m_scenes
	auto it = std::find_if(m_scenes.begin(), m_scenes.end(),
		[&](const SceneInfo& info) {
			return info.name == sceneName;
		});

	if (it != m_scenes.end())
	{
		// Recreate the scene using the factory
		m_currentScene = it->factory();
		engine::Singleton::reset(m_currentScene->getSceneSettings());
		return true;
	}

	return false;
}

void engine::SceneManager::clearScenes()
{
	m_scenes.clear();
	m_currentScene.reset();
}

bool engine::SceneManager::unloadCurrentScene()
{
	if (m_currentScene)
	{
		// Call exit() to clean up resources
		m_currentScene->exit();

		// Reset the shared_ptr to destroy the scene
		m_currentScene.reset(); // force destroying the scene and calling destructor

		// The weak_ptr in m_scenes is now expired
		return true;
	}

	return false;
}

void engine::SceneManager::clean()
{

}

engine::SceneManager::~SceneManager()
{
	logger.trace("SceneManager destructor called");
}
