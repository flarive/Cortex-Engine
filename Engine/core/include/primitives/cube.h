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
                {"Width", EditorProperty { getWidth(), 0.0f, 10.0f, 0.01f, "%.3f" }},
                {"Height", EditorProperty { getHeight(), 0.0f, 10.0f, 0.01f, "%.3f" }},
                {"Depth", EditorProperty { getDepth(), 0.0f, 10.0f, 0.01f, "%.3f" }},
                {"UV scale", EditorProperty { getUvScale(), 0.0f, 10.0f, 0.01f, "%.3f" }},
                {"Cast shadows", EditorProperty { canCastShadows(), 0.0f, 10.0f, 0.01f, "%.3f" }},
                {"Receive shadows", EditorProperty { canReceiveShadows(), 0.0f, 10.0f, 0.01f, "%.3f" }}
            };
        }
        std::unordered_map<std::string, std::function<void(float)>> getPropertySetters() override {
            return {
                {"width", [this](float value) { getWidth() = value; }},
                {"height", [this](float value) { getHeight() = value; }},
                {"depth", [this](float value) { getDepth() = value; }},
                {"uvscale", [this](float value) { getUvScale() = value; }},
                {"canCastShadows", [this](float value) { canCastShadows() = value; }},
                {"canReceiveShadows", [this](float value) { canReceiveShadows() = value; }}
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
