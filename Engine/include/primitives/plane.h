#pragma once

#include "primitive.h"

#include "../shader.h"
#include "../texture.h"

namespace engine
{
    class Plane : public Primitive
    {
    public:
        Plane();


        void setup() override;


        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, float rotationAngle = 0.0f, const glm::vec3& rotationAxis = glm::vec3(0.0f, 0.0f, 0.0f));
    };
}
