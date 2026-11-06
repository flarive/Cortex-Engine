#pragma once

#include "primitive.h"

namespace engine
{
    class Triangle final : public Primitive
    {
    public:
        Triangle(const glm::vec3& position = glm::vec3(0.0f));
        Triangle(float width, float height, const glm::vec3& position = glm::vec3(0.0f));
        ~Triangle() override;

        void setup() override;
        void setup(const std::shared_ptr<Material>& material);
        void setup(const std::shared_ptr<Material>& material, const UvMapping& uv);

        ordered_map<std::string, EditorProperty> getPublicProperties() override {
            return {
                { "width", EditorProperty{ "Width", getWidth(), editable, 0.0f, 10.0f, 0.01f, "%.3f" } },
                { "height", EditorProperty{ "Height", getHeight(), editable, 0.0f, 10.0f, 0.01f, "%.3f" } },
                { "uvscale", EditorProperty{ "UV scale", getUvScale(), editable, 0.0f, 10.0f, 0.01f, "%.3f" } },
                { "canCastShadows", EditorProperty{ "Cast shadows", canCastShadows(), editable, 0.0f, 10.0f, 0.01f, "%.3f" } },
                { "canReceiveShadows", EditorProperty{ "Receive shadows", canReceiveShadows(), editable, 0.0f, 10.0f, 0.01f, "%.3f" } }
            };
        }
        std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() override {
            return {
                { "width", [this](EditorPropertyValue value) { getWidth() = *(std::get_if<float>(&value)); } },
                { "height", [this](EditorPropertyValue value) { getHeight() = *(std::get_if<float>(&value)); } },
                { "uvscale", [this](EditorPropertyValue value) { getUvScale() = *(std::get_if<float>(&value)); } },
                { "canCastShadows", [this](EditorPropertyValue value) { canCastShadows() = *(std::get_if<bool>(&value)); } },
                { "canReceiveShadows", [this](EditorPropertyValue value) { canReceiveShadows() = *(std::get_if<bool>(&value)); } }
            };
        }

        float& getWidth() { return m_width; }
        void setWidth(float width) { m_width = width; }

        float& getHeight() { return m_height; }
        void setHeight(float height) { m_height = height; }


        void draw(std::unique_ptr<ShaderBase>& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform) override;

        std::vector<Vertex> generateVertices() override;

        PrimitiveType getTypeID() const override
        {
            return PrimitiveType::triangle;
        }

        void clean() override;

        

    private:
        void geometrySetup();

		float m_width{ 1.0f };
		float m_height{ 1.0f };
        
    };
}