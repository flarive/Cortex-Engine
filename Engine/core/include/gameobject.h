#pragma once



#include <glad/glad.h>
#include <glm/glm.hpp>

#include "misc/noncopyable.h"

#include <string>

// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within GameObject.

namespace engine
{
	class GameObject : private NonCopyable
	{
	public:
		std::string name;
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;

		GameObject(const std::string& name = "GameObject",
			const glm::vec3& position = glm::vec3(0.0f),
			const glm::vec3& rotation = glm::vec3(0.0f),
			const glm::vec3& scale = glm::vec3(1.0f))
			: name(name), position(position), rotation(rotation), scale(scale) {
		}

		virtual void Init() {
			// Init logic for the game object
		}

		virtual void Update(float deltaTime) {
			// Update logic for the game object
		}

		virtual void Render() {
			// Render logic for the game object
		}

		virtual ~GameObject() = default;
	};
}


//
//#include <vector>
//#include <memory>
//#include <glm/glm.hpp>
//#include "GameObject.h" // Include your GameObject header file
//
//class Scene {
//public:
//    std::vector<std::shared_ptr<GameObject>> gameObjects;
//
//    template <typename T, typename... Args>
//    std::shared_ptr<T> addGameObject(Args&&... args) {
//        static_assert(std::is_base_of<GameObject, T>::value, "T must be derived from GameObject");
//        std::shared_ptr<T> gameObject = std::make_shared<T>(std::forward<Args>(args)...);
//        gameObjects.push_back(gameObject);
//        return gameObject;
//    }
//
//    void Update(float deltaTime) {
//        for (auto& gameObject : gameObjects) {
//            gameObject->Update(deltaTime);
//        }
//    }
//
//    void Render() {
//        for (auto& gameObject : gameObjects) {
//            gameObject->Render();
//        }
//    }
//};
//
//// Example usage
//int main() {
//    Scene myScene;
//    auto camera = myScene.addGameObject<Camera>(glm::vec3(0.0f, 0.0f, 5.0f));
//    auto sprite = myScene.addGameObject<Sprite>("MySprite", glm::vec3(1.0f, 2.0f, 0.0f));
//
//    // Main loop
//    float deltaTime = 0.016f; // Example delta time
//    while (true) {
//        myScene.Update(deltaTime);
//        myScene.Render();
//    }
//
//    return 0;
//}
