#pragma once

#include "../common_defines.h"

#include "../shader.h"
#include "../materials/material.h"
#include "../primitives/primitive.h"
#include "../uvmapping.h"

namespace engine
{
    class Cone final : public Primitive
    {
    public:
        Cone(float _radius = 1.0f, float _height = 3.0f, const glm::vec3& _position = glm::vec3());
        ~Cone() override;

		void setup() override;
        void setup(const std::shared_ptr<Material>& material) override;
        void setup(const std::shared_ptr<Material>& material, const UvMapping& uv) override;

        ordered_map<std::string, EditorProperty> getPublicProperties() override {
            return {
                {"radius", EditorProperty { "Radius", getRadius(), editable, 0.0f, 10.0f, 0.01f, "%.3f"}},
                {"height", EditorProperty { "Height", getHeight(), editable, 0.0f, 10.0f, 0.01f, "%.3f" }},
                {"uvscale", EditorProperty { "UV scale", getUvScale(), editable, 0.0f, 10.0f, 0.01f, "%.3f" }},
                {"canCastShadows", EditorProperty { "Cast shadows", canCastShadows(), editable, 0.0f, 10.0f, 0.01f, "%.3f" }},
                {"canReceiveShadows", EditorProperty { "Receive shadows", canReceiveShadows(), editable, 0.0f, 10.0f, 0.01f, "%.3f" }}
            };
        }

        std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() override {
            return {
                { "radius", [this](EditorPropertyValue value) { getRadius() = *(std::get_if<float>(&value)); } },
                { "height", [this](EditorPropertyValue value) { getHeight() = *(std::get_if<float>(&value)); } },
                { "uvscale", [this](EditorPropertyValue value) { getUvScale() = *(std::get_if<float>(&value)); } },
                { "canCastShadows", [this](EditorPropertyValue value) { canCastShadows() = *(std::get_if<bool>(&value)); } },
                { "canReceiveShadows", [this](EditorPropertyValue value) { canReceiveShadows() = *(std::get_if<bool>(&value)); } }
            };
        }

        float& getRadius() { return m_radius; }
        void setRadius(float radius) { m_radius = radius; }

        float& getHeight() { return m_height; }
        void setHeight(float height) { m_height = height; }

        std::vector<Vertex> generateVertices() override;

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform) override;

        PrimitiveType getTypeID() const override
        {
            return PrimitiveType::cone;
        }

		void clean() override;

    private:
        float m_radius{ 1.0f };
        float m_height{ 2.0f };

        
        void geometrySetup();

        unsigned int indexCount{};
    };
}
