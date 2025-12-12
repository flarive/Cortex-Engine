#pragma once


#include "component.h"

#include "../models/model.h"
#include "../frustrum.h"
#include "../bounding_volume.h"
#include "../aabb.h"

#include "../primitives/cube.h"
#include "../shader.h"

namespace engine
{
	class ModelComponent final : public ComponentBase<ModelComponent>
	{
	public:

		ModelComponent() = default;
		ModelComponent(std::shared_ptr<Model> model);
		~ModelComponent() = default;

		void init(Transform& transform) override;
		void update(float deltaTime, Transform& transform) override;

		void draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform, AABB* boundingVolume = nullptr) override;

		std::shared_ptr<Model> getModel()
		{
			return m_model;
		}

		static ComponentType getStaticTypeID() {
			return ComponentType::model;
		}

		std::string getName() override
		{
			return "Model";
		}

		engine::AABB* getBoundingVolume() override;

		ordered_map<std::string, EditorProperty> getPublicProperties() override;
		std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() override;

		void setProperty(const std::string& key, EditorPropertyValue value) override;

	private:

		std::shared_ptr<Model> m_model{};
		std::unique_ptr<AABB> m_boundingVolume{};

		std::unique_ptr<Cube> m_debug_boundingBox{};

		Shader m_lightDebugShader{};

		bool DISPLAY_DEBUG_BOUNDING_BOX{ true };

		std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> m_propertySetters{};

		AABB generateBoundingVolume(const std::shared_ptr<Model> model);
	};
}