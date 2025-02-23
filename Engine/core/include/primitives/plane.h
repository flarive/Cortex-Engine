#pragma once

#include "../shader.h"
#include "../materials/material2.h"
#include "../primitives/primitive.h"

namespace engine
{
    class Plane// : public Primitive
    {
    public:
        Plane() = default;
        ~Plane() = default;

        void setup(const std::shared_ptr<engine::Material2>& material);
        void setup(const std::shared_ptr<engine::Material2>& material, const UvMapping& uv);

        //void setup(const glm::uvec3& color) override;
        //void setup(const engine::Material& material) override;
        //void setup(const Material& material, const UvMapping& uv) override;

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, float rotationAngle = 0.0f, const glm::vec3& rotationAxis = glm::vec3(0.0f, 0.0f, 0.0f));

        void clean();

    private:
        void setup();// override;

        std::shared_ptr<engine::Material2> m_material;

        unsigned int m_VBO{}, m_VAO{}, m_EBO{};

        float m_uvScale{ 0.5f };


        unsigned int indexCount{};
    };
}
