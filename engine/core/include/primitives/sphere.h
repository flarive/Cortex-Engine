#pragma once

#include "../shader.h"
#include "../materials/material.h"
#include "../primitives/primitive.h"

namespace engine
{
    class Sphere : public Primitive
    {
    public:
        Sphere() = default;
        ~Sphere() = default;

        void setup(const glm::uvec3& color) override;

        void setup(const engine::Material& material) override;

        void setup(const Material& material, const UvMapping& uv) override;

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, float rotationAngle = 0.0f, const glm::vec3& rotationAxis = glm::vec3(0.0f, 0.0f, 0.0f));

    private:
        void setup() override;

        unsigned int indexCount{};
    };
}
