#pragma once

#include "ui.h"

namespace engine
{
    class UISprite final : public UIBase
    {
    public:
        
        unsigned int width{};
        unsigned int height{};
        
        // Constructor (inits shaders/shapes)
        UISprite() = default;
        // Destructor
        ~UISprite();
        void setup(GLFWwindow* window, const std::string& filepath);
        // Renders a defined quad textured with given sprite
        void draw(glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f, const Color& color = Colors::White);


        void clean() override;

    private:
        std::string m_filepath{};
        static Shader m_spriteShader; // Shared across all instances
        unsigned int m_texture_id{};
        // Initializes and configures the quad's buffer and vertex attributes
        void initRenderData();
    };
}
