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

        std::vector<KeyValuePair> getPublicProperties() override {
            return {
                {"width", m_width},
                {"height", m_height},
                {"depth", m_depth},
                {"uvscale", getUvScale()}
            };
        }
        std::unordered_map<std::string, std::function<void(float)>> getPropertySetters() override {
            return {
                {"width", [this](float value) { m_width = value; }},
                {"height", [this](float value) { m_height = value; }},
                {"depth", [this](float value) { m_depth = value; }},
                {"uvscale", [this](float value) { getUvScale() = value; }}
            };
        }

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
