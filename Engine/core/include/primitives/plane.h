#pragma once

#include "../shader.h"
#include "../materials/material.h"
#include "../primitives/primitive.h"

namespace engine
{
    class Plane final : public Primitive
    {
    public:
        Plane(bool _flipNormals = true, const glm::vec3& _position = glm::vec3());
        ~Plane() = default;

        void setup() override;
        void setup(const std::shared_ptr<Material>& material) override;
        void setup(const std::shared_ptr<Material>& material, const UvMapping& uv) override;

        ordered_map<std::string, EditorProperty> getPublicProperties() override {
            return {
                {"UV scale", EditorProperty { getUvScale(), 0.0f, 10.0f, 0.01f, "%.3f" }},
                {"Cast shadows", EditorProperty { canCastShadows(), 0.0f, 10.0f, 0.01f, "%.3f" }},
                {"Receive shadows", EditorProperty { canReceiveShadows(), 0.0f, 10.0f, 0.01f, "%.3f" }}
            };
        }
        std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() override {
            return {
                { "uvscale", [this](EditorPropertyValue value) { getUvScale() = *(std::get_if<float>(&value)); } },
                { "canCastShadows", [this](EditorPropertyValue value) { canCastShadows() = *(std::get_if<bool>(&value)); } },
                { "canReceiveShadows", [this](EditorPropertyValue value) { canReceiveShadows() = *(std::get_if<bool>(&value)); } }
                //{"uvscale", [this](float value) { getUvScale() = value; }},
                //{"canCastShadows", [this](float value) { canCastShadows() = value; }},
                //{"canReceiveShadows", [this](float value) { canReceiveShadows() = value; }}
            };
        }

        std::vector<Vertex> generateVertices() override;

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform) override;

        PrimitiveType getTypeID() const override
        {
            return PrimitiveType::plane;
        }

        void clean() override;

    private:
        bool m_flipNormals{ true };
        
        void geometrySetup();

        void drawDebugNormals(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix);

        DebugDraw m_debugDrawLine{};
    };
}
