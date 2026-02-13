#pragma once


#include "component.h"

#include "../primitives/primitive.h"
#include "../frustrum.h"
#include "../bounding_volume.h"
#include "../aabb.h"

#include "../debug/debug_cube.h"
#include "../shader.h"

namespace engine
{
	class PrimitiveComponent final : public ComponentBase<PrimitiveComponent>
	{
	public:

		PrimitiveComponent() = default;
		PrimitiveComponent(std::shared_ptr<Primitive> primitive);
		~PrimitiveComponent() = default;


		void init(Transform& transform) override;
		void update(float deltaTime, Transform& transform) override;

		void draw(const glm::mat4& projection, const glm::mat4& view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform, AABB* boundingVolume = nullptr) override;

		std::shared_ptr<Primitive> getPrimitive()
		{
			return m_primitive;
		}


		static ComponentType getStaticTypeID() {
			return ComponentType::primitive;
		}

		std::string getName() override
		{
			return "Primitive";
		}

		engine::AABB* getBoundingVolume() override;

		ordered_map<std::string, engine::EditorProperty> getPublicProperties() override;
		std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() override;

		void setProperty(const std::string& key, EditorPropertyValue value);

		void setEnabled(bool enabled) override;

	private:

		std::shared_ptr<Primitive> m_primitive{};
		std::unique_ptr<AABB> m_boundingVolume{};

		std::unique_ptr<DebugCube> m_debug_boundingBox{};



		std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> m_propertySetters{};

		AABB generateBoundingVolume(const std::shared_ptr<Primitive> primitive);
	};
}