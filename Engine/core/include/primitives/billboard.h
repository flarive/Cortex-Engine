#pragma once

#include "../common_defines.h"

#include "primitive.h"

#include "../shader.h"
#include "../materials/material.h"

namespace engine
{
    class Billboard : public Primitive
    {
    public:
        Billboard();

        void setup(const glm::uvec3& color) override;

        void setup(const engine::Material& material) override;

        void setup(const Material& material, const UvMapping& uv) override;
        
        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, float rotationAngle = 0.0f, const glm::vec3& rotationAxis = glm::vec3(0.0f, 0.0f, 0.0f));

    private:
        void setup() override;
    };
}
