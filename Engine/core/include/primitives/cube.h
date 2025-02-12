#pragma once

#include "../shader.h"
#include "../materials/material.h"
#include "../primitives/primitive.h"

namespace engine
{
    class Cube : public Primitive
    {
    public:
        Cube() = default;
        ~Cube() = default;

        void setup(const glm::uvec3& color) override;
        void setup(const engine::Material& material) override;
        void setup(const engine::Material& material, const UvMapping& uv) override;

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, float rotationAngle, const glm::vec3& rotationAxis);

    private:
        void setup() override;
    };
}
