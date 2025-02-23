#pragma once

#include "../common_defines.h"

#include "../shader.h"
#include "../materials/material2.h"
#include "../primitives/primitive.h"
#include "../uvmapping.h"

namespace engine
{
    class Sphere : public Primitive
    {
    public:
        Sphere() = default;
        ~Sphere() = default;

        void setup(const std::shared_ptr<engine::Material2>& material) override;
        void setup(const std::shared_ptr<engine::Material2>& material, const UvMapping& uv) override;

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, float rotationAngle = 0.0f, const glm::vec3& rotationAxis = glm::vec3(0.0f, 0.0f, 0.0f));

        //void clean();

    private:
        //std::shared_ptr<engine::Material2> m_material;

        //unsigned int m_VBO{}, m_VAO{}, m_EBO{};
        //
        //float m_uvScale{ 0.5f };
        
        void setup();

        

        unsigned int indexCount{};
    };
}
