#pragma once

#include "../shader.h"
#include "../materials/material.h"
#include "../primitives/primitive.h"

namespace engine
{
    class Cube final : public Primitive
    {
    public:
        Cube(const glm::vec3& _position = glm::vec3());
        Cube(const float& _size, const glm::vec3& _position = glm::vec3());
        Cube(const float& _width, const float& _height, const float& _depth, const glm::vec3& _position = glm::vec3());
        ~Cube() = default;

		void setup() override;
        void setup(const std::shared_ptr<Material>& material) override;
        void setup(const std::shared_ptr<Material>& material, const UvMapping& uv) override;

        ordered_map<std::string, EditorProperty> getPublicProperties() override {
            return {
                {"width", EditorProperty { "Width", getWidth(), editable, 0.0f, 10.0f, 0.01f, "%.3f" }},
                {"height", EditorProperty { "Height", getHeight(), editable, 0.0f, 10.0f, 0.01f, "%.3f" }},
                {"depth", EditorProperty { "Depth", getDepth(), editable, 0.0f, 10.0f, 0.01f, "%.3f" }},
                {"uvscale", EditorProperty { "UV scale", getUvScale(), editable, 0.0f, 10.0f, 0.01f, "%.3f" }},
                {"canCastShadows", EditorProperty { "Cast shadows", canCastShadows(), editable, 0.0f, 10.0f, 0.01f, "%.3f" }},
                {"canReceiveShadows", EditorProperty { "Receive shadows", canReceiveShadows(), editable, 0.0f, 10.0f, 0.01f, "%.3f" }}
            };
        }
        std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() override {
            return {
                { "width", [this](EditorPropertyValue value) { getWidth() = *(std::get_if<float>(&value)); } },
                { "height", [this](EditorPropertyValue value) { getHeight() = *(std::get_if<float>(&value)); } },
                { "depth", [this](EditorPropertyValue value) { getDepth() = *(std::get_if<float>(&value)); } },
                { "uvscale", [this](EditorPropertyValue value) { getUvScale() = *(std::get_if<float>(&value)); } },
                { "canCastShadows", [this](EditorPropertyValue value) { canCastShadows() = *(std::get_if<bool>(&value)); } },
                { "canReceiveShadows", [this](EditorPropertyValue value) { canReceiveShadows() = *(std::get_if<bool>(&value)); } }
            };
        }

        float& getWidth() { return m_width; }
        void setWidth(float width) { m_width = width; }

        float& getHeight() { return m_height; }
        void setHeight(float height) { m_height = height; }

        float& getDepth() { return m_depth; }
        void setDepth(float depth) { m_depth = depth; }

        std::vector<Vertex> generateVertices() override;

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform) override;

        PrimitiveType getTypeID() const override
        {
            return PrimitiveType::cube;
        }

        void clean() override;

    private:
        
        float m_width{ 1.0f };
        float m_height{ 1.0f };
        float m_depth{ 1.0f };
        
        void geometrySetup();

        void drawDebugNormals(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix);

        DebugDraw m_debugDrawLine{};
    };
}
