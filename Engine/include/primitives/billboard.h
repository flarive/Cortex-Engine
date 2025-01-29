#pragma once

#include "../common_defines.h"

#include "primitive.h"

#include "../shader.h"
#include "../texture.h"

namespace engine
{
    class Billboard : public Primitive
    {
    public:
        Billboard();

        void setup() override;
        
        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, float rotationAngle, const glm::vec3& rotationAxis);
    };
}
