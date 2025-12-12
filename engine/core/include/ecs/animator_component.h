#pragma once


#include "component.h"

#include "../models/model.h"
#include "../frustrum.h"
#include "../bounding_volume.h"
#include "../aabb.h"

#include "../models/animator.h"
#include "../shader.h"

#include <variant>

namespace engine
{
	class AnimatorComponent final : public ComponentBase<AnimatorComponent>
	{
	public:

		AnimatorComponent() = default;
		AnimatorComponent(std::shared_ptr<Animator> model);
		~AnimatorComponent() = default;

		void init(Transform& transform) override;
		void update(float deltaTime, Transform& transform) override;

		void draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform, AABB* boundingVolume = nullptr) override;

		std::shared_ptr<Animator> getModel()
		{
			return m_animator;
		}

		static ComponentType getStaticTypeID() {
			return ComponentType::animator;
		}

		std::string getName() override
		{
			return "Animator";
		}

		engine::AABB* getBoundingVolume() override;

		ordered_map<std::string, EditorProperty> getPublicProperties() override;
		std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() override;

		void setProperty(const std::string& key, EditorPropertyValue value) override;

	private:

		std::shared_ptr<Animator> m_animator{};
		std::unique_ptr<AABB> m_boundingVolume{};


		AABB generateBoundingVolume(const std::shared_ptr<Animator> animator);
	};
}