#pragma once


#include "component.h"

#include "../primitives/primitive.h"
#include "../frustrum.h"
#include "../bounding_volume.h"
#include "../aabb.h"

namespace engine
{
	class PrimitiveComponent : public ComponentBase<PrimitiveComponent>
	{
	public:

		PrimitiveComponent() = default;
		PrimitiveComponent(std::shared_ptr<Primitive> primitive);
		~PrimitiveComponent() = default;


		void init() override;
		void update(Transform& transform) override;
		void draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& transform) override;

		

		std::shared_ptr<Primitive> getPrimitive()
		{
			return m_primitive;
		}


		static unsigned int getStaticTypeID() {
			return 3;
		}

	private:

		std::shared_ptr<Primitive> m_primitive{};
		std::unique_ptr<AABB> m_boundingVolume{};

		engine::AABB generateAABB(const std::shared_ptr<Primitive> primitive);

		std::unique_ptr<engine::AABB> getBoundingVolume();
	};
}