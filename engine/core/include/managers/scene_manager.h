#pragma once

#include "../common_defines.h"

namespace engine
{
	class App;
	class Scene;

	struct SceneInfo final
	{
		std::string name;
		std::function<std::shared_ptr<Scene>()> factory;
		std::shared_ptr<Scene> instance; // Optional: Cache the instance if needed
	};

	
	class SceneManager final
	{
	public:
		SceneManager() = default;
		~SceneManager();

		std::shared_ptr<engine::Scene> setCurrentScene(unsigned int sceneIndex);
		bool setCurrentScene(const std::string& sceneName);

		std::shared_ptr<engine::Scene> getCurrentScene();


		bool shouldUnloadScene() const { return m_shouldUnloadScene; }
		void requestSceneUnload() { m_shouldUnloadScene = true; }

		void clearScenes();

		// Add a scene with a factory function
		template <typename T>
		void addScene(const std::string& name, std::weak_ptr<App> app)
		{
			bool empty = m_scenes.empty();
			// Capture app by value (copies the weak_ptr) but lock it in the lambda
			m_scenes.push_back({
				name,
				[app, name]() {
					// Lock the weak_ptr to get a shared_ptr
					if (auto lockedApp = app.lock()) {
						return std::make_shared<T>(name, lockedApp);
					}
					else {
						// Handle the case where the App is destroyed
						throw std::runtime_error("App object has been destroyed");
					}
				},
				nullptr // No instance created yet
				});

			if (empty)
			{
				// Create the first scene
				m_currentScene = m_scenes[0].factory();
			}
		}

		size_t getSceneCount() { return m_scenes.size(); }


		bool unloadCurrentScene(); // shouldn't be called directly, use requestSceneUnload instead
		void clean();

	private:
		std::vector<SceneInfo> m_scenes{};
		std::shared_ptr<engine::Scene> m_currentScene{};

		bool m_shouldUnloadScene{ false };
	};
}