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
        float radius{ 1.0f };
        float height{ 2.0f };

        Cone(const glm::vec3& _position = glm::vec3());
        ~Cone() = default;

		void setup() override;
        void setup(const std::shared_ptr<Material>& material) override;
        void setup(const std::shared_ptr<Material>& material, const UvMapping& uv) override;

        std::vector<KeyValuePair> getPublicProperties() override {
            return {
                {"radius", radius},
                {"height", height},
                {"uvscale", getUvScale()}
            };
        }
        std::unordered_map<std::string, std::function<void(float)>> getPropertySetters() override {
            return {
                {"radius", [this](float value) { radius = value; }},
                {"height", [this](float value) { height = value; }},
                {"uvscale", [this](float value) { getUvScale() = value; }}
            };
        }


        std::vector<Vertex> generateVertices() override;

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform) override;

        PrimitiveType getTypeID() const override
        {
            return PrimitiveType::cone;
        }

		void clean() override;

    private:
        void geometrySetup();

        unsigned int indexCount{};
    };
}
