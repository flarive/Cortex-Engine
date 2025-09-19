#pragma once


#include "component.h"

#include "../model.h"
#include "../frustrum.h"
#include "../bounding_volume.h"
#include "../aabb.h"

namespace engine
{
	class ModelComponent : public ComponentBase<ModelComponent>
	{
	public:

		ModelComponent() = default;
		ModelComponent(std::shared_ptr<Model> model);
		~ModelComponent() = default;

		void init(Transform& transform) override;
		void update(Transform& transform) override;

		void draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& transform) override;
		void draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation) override;

		std::shared_ptr<Model> getModel()
		{
			return m_model;
		}


		static ComponentType getStaticTypeID() {
			return ComponentType::model;
		}

		engine::AABB* getBoundingVolume() override;

	private:

		std::shared_ptr<Model> m_model{};
		std::unique_ptr<AABB> m_boundingVolume{};


		AABB generateBoundingVolume(const std::shared_ptr<Model> model);
	};
}