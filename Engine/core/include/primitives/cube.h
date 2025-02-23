#pragma once

#include "../shader.h"
#include "../materials/material2.h"
#include "../primitives/primitive.h"

namespace engine
{
    class Cube// : public Primitive
    {
    public:
        Cube() = default;
        ~Cube() = default;

        //void setup(const glm::uvec3& color) override;
        //void setup(const engine::Material& material) override;
        //void setup(const engine::Material& material, const UvMapping& uv) override;

        void setup(const std::shared_ptr<engine::Material2>& material);
        void setup(const std::shared_ptr<engine::Material2>& material, const UvMapping& uv);

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, float rotationAngle, const glm::vec3& rotationAxis);

        void clean();

    private:

        std::shared_ptr<engine::Material2> m_material;

        unsigned int m_VBO{}, m_VAO{}, m_EBO{};

        float m_uvScale{ 0.5f };

        void setup();// override;

        unsigned int m_cubemapTexture{};

        unsigned int indexCount{};
    };
}
