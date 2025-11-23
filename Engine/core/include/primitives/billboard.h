#pragma once

#include "../common_defines.h"

#include "../shader.h"
#include "../materials/material.h"
#include "../primitives/primitive.h"

namespace engine
{
    class Billboard final : public Primitive
    {
    public:
        Billboard(const glm::vec3& _position = glm::vec3());
        ~Billboard() = default;

		void setup() override;
        void setup(const std::shared_ptr<Material>& material) override;
        void setup(const std::shared_ptr<Material>& material, const UvMapping& uv) override;

        ordered_map<std::string, std::variant<int, std::string, float, bool>> getPublicProperties() override {
            return {
                {"uvscale", getUvScale()},
                {"canCastShadows", canCastShadows()},
                {"canReceiveShadows", canReceiveShadows()}
            };
        }

        std::unordered_map<std::string, std::function<void(float)>> getPropertySetters() override {
            return {
                {"uvscale", [this](float value) { getUvScale() = value; }},
                {"canCastShadows", [this](float value) { canCastShadows() = value; }},
                {"canReceiveShadows", [this](float value) { canReceiveShadows() = value; }}
            };
        }


        std::vector<Vertex> generateVertices() override;
        
        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform) override;


        PrimitiveType getTypeID() const override
        {
            return PrimitiveType::billboard;
        }

        void clean() override;

    private:
        void geometrySetup();
    };
}
