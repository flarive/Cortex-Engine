#pragma once


#include "component.h"

#include "../primitives/plane.h"
#include "../primitives/cube.h"
#include "../primitives/sphere.h"
#include "../primitives/cylinder.h"
#include "../primitives/cone.h"
#include "../primitives/billboard.h"

namespace engine
{
	class PrimitiveComponent : public ComponentBase<PrimitiveComponent>
	{
	public:

		PrimitiveComponent() = default;
		PrimitiveComponent(std::shared_ptr<Primitive> primitive);
		~PrimitiveComponent() = default;


		void init() override;
		void update() override;
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
	};
}