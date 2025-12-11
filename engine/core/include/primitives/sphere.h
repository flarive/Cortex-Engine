#pragma once

#include "../common_defines.h"

#include "../shader.h"
#include "../materials/material.h"
#include "../primitives/primitive.h"
#include "../uvmapping.h"

namespace engine
{
    class Sphere final : public Primitive
    {
    public:
        

        Sphere(const glm::vec3& _position = glm::vec3());
        ~Sphere() = default;

		void setup() override;
        void setup(const std::shared_ptr<Material>& material) override;
        void setup(const std::shared_ptr<Material>& material, const UvMapping& uv) override;

        ordered_map<std::string, EditorProperty> getPublicProperties() override {
            return {
                {"radius", EditorProperty { getRadius(), 0.0f, 10.0f, 0.01f, "%.3f" }},
                {"uvscale", EditorProperty { getUvScale(), 0.0f, 10.0f, 0.01f, "%.3f" }},
                {"canCastShadows", EditorProperty { canCastShadows(), 0.0f, 10.0f, 0.01f, "%.3f" }},
                {"canReceiveShadows", EditorProperty { canReceiveShadows(), 0.0f, 10.0f, 0.01f, "%.3f" } }
            };
        }
        std::unordered_map<std::string, std::function<void(float)>> getPropertySetters() override {
            return {
                {"radius", [this](float value) { getRadius() = value; }},
                {"uvscale", [this](float value) { getUvScale() = value; }},
                {"canCastShadows", [this](float value) { canCastShadows() = value; }},
                {"canReceiveShadows", [this](float value) { canReceiveShadows() = value; }}
            };
        }


        float& getRadius() { return m_radius; }
        void setRadius(float radius) { m_radius = radius; }

        std::vector<Vertex> generateVertices() override;

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform) override;

        PrimitiveType getTypeID() const override
        {
            return PrimitiveType::sphere;
        }

        void clean() override;

    private:
        float m_radius{ 1.0f };
        
        void geometrySetup();

        unsigned int indexCount{};
    };
}
