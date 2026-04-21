#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"
#include "../misc/colors.h"

#include "../shader.h"

#include <glm/glm.hpp>

namespace engine
{
    class UISprite final : private NonCopyable
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


        void clean();

    private:
        // Render state
        GLFWwindow* m_window{};
        std::string m_filepath{};
        static Shader m_spriteShader; // Shared across all instances
        unsigned int m_texture_id{};
        unsigned int m_quadVAO{};
        // Initializes and configures the quad's buffer and vertex attributes
        void initRenderData();
    };
}
