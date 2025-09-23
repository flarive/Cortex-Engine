#pragma once

#include "../common_defines.h"

#include "../shader.h"
#include "../materials/material.h"
#include "../primitives/primitive.h"
#include "../uvmapping.h"

namespace engine
{
    class Cylinder final : public Primitive
    {
    public:
        float radius{ 1.0f };
        float height{ 2.0f };

        Cylinder(const glm::vec3& _position = glm::vec3());
        ~Cylinder() = default;

        void setup(const std::shared_ptr<Material>& material) override;
        void setup(const std::shared_ptr<Material>& material, const UvMapping& uv) override;

        std::vector<Vertex> generateVertices() override;

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::mat4 transformMatrix, Transform& localTransform) override;

    private:
        void setup();

        unsigned int indexCount{};
    };
}
