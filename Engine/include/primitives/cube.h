#pragma once

#include "primitive.h"

#include "../shader.h"

namespace engine
{
    class Cube : public Primitive
    {
    public:
        Cube();

        void setup() override;
        
        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, float rotationAngle, const glm::vec3& rotationAxis);
    };
}
