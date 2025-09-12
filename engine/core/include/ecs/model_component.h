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

		void init() override;
		void update(Transform& transform) override;
		void draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& transform) override;

		std::shared_ptr<Model> getModel()
		{
			return m_model;
		}


		static unsigned int getStaticTypeID() {
			return 4;
		}

	private:

		std::shared_ptr<Model> m_model{};
		std::unique_ptr<AABB> m_boundingVolume{};


		AABB generateAABB(const std::shared_ptr<Model> model);

		std::unique_ptr<AABB> getBoundingVolume() override;
	};
}