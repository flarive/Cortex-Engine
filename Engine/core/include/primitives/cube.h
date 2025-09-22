#pragma once

#include "../shader.h"
#include "../materials/material.h"
#include "../primitives/primitive.h"

namespace engine
{
    class Cube final : public Primitive
    {
    public:
        Cube() = default;
        ~Cube() = default;

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
