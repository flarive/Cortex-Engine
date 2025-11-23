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
        ~Cone() = default;

		void setup() override;
        void setup(const std::shared_ptr<Material>& material) override;
        void setup(const std::shared_ptr<Material>& material, const UvMapping& uv) override;

        ordered_map<std::string, std::variant<int, std::string, float, bool>> getPublicProperties() override {
            return {
                {"radius", getRadius()},
                {"height", getHeight()},
                {"uvscale", getUvScale()},
                {"canCastShadows", canCastShadows()},
                {"canReceiveShadows", canReceiveShadows()}
            };
        }

        std::unordered_map<std::string, std::function<void(float)>> getPropertySetters() override {
            return {
                {"radius", [this](float value) { getRadius() = value; }},
                {"height", [this](float value) { getHeight() = value; }},
                {"uvscale", [this](float value) { getUvScale() = value; }},
                {"canCastShadows", [this](float value) { canCastShadows() = value; }},
                {"canReceiveShadows", [this](float value) { canReceiveShadows() = value; }}
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
