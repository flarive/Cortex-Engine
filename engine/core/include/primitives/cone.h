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

        void setup(const std::shared_ptr<Material>& material) override;
        void setup(const std::shared_ptr<Material>& material, const UvMapping& uv) override;

        std::vector<Vertex> generateVertices() override;

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::mat4& transformMatrix, Transform& localTransform) override;

        PrimitiveType getTypeID() const override
        {
            return PrimitiveType::cone;
        }

    private:
        void setup();

        unsigned int indexCount{};
    };
}
