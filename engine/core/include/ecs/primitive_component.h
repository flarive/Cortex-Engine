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


		void init(Transform& transform) override;
		void update(Transform& transform) override;

		void draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& transform) override;
		void draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation) override;

		std::shared_ptr<Primitive> getPrimitive()
		{
			return m_primitive;
		}


		static ComponentType getStaticTypeID() {
			return ComponentType::primitive;
		}

		std::unique_ptr<AABB> getBoundingVolume() override;

	private:

		std::shared_ptr<Primitive> m_primitive{};
		std::unique_ptr<AABB> m_boundingVolume{};

		AABB generateBoundingVolume(const std::shared_ptr<Primitive> primitive);
	};
}