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

        Cylinder() = default;
        ~Cylinder() = default;

        void setup(const std::shared_ptr<Material>& material) override;
        void setup(const std::shared_ptr<Material>& material, const UvMapping& uv) override;

        std::vector<Vertex> generateVertices() override;

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation = glm::vec3(0.0f, 0.0f, 0.0f)) override;

        void draw(Shader& shader, const glm::mat4 model) override;

    private:
        void setup();

        unsigned int indexCount{};
    };
}
