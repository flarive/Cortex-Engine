#pragma once

#include "primitive.h"

#include "../shader.h"

namespace engine
{
    class Plane : public Primitive
    {
    public:
        Plane();

        void setup(const glm::uvec3& color) override;

        void setup(const std::string& diffuseTexPath, const std::string& specularTexPath = "", const std::string& normalTexPath = "") override;

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, float rotationAngle = 0.0f, const glm::vec3& rotationAxis = glm::vec3(0.0f, 0.0f, 0.0f));

    private:
        void setup() override;
    };
}
