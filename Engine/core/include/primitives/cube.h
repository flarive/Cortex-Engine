#pragma once

#include "primitive.h"

#include "../shader.h"
#include "../materials/material.h"

namespace engine
{
    class Cube : public Primitive
    {
    public:
        Cube();

        void setup(const glm::uvec3& color) override;

        void setup(const Material& material) override;

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, float rotationAngle, const glm::vec3& rotationAxis);

    private:
        void setup() override;
    };
}
