#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"

#include "../shader.h"

#include <glm/glm.hpp>

namespace engine
{
    class Sprite
    {
    public:
        
        unsigned int width{};
        unsigned int height{};
        
        // Constructor (inits shaders/shapes)
        Sprite() = default;
        // Destructor
        ~Sprite();
        void setup(const std::string& filepath, int screenWidth, int screenHeight);
        // Renders a defined quad textured with given sprite
        void draw(glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));


        

    private:
        // Render state
        std::string m_filepath{};
        Shader m_spriteShader{};
        unsigned int m_texture_id{ 0 };
        unsigned int m_quadVAO{ 0 };
        // Initializes and configures the quad's buffer and vertex attributes
        void initRenderData();
    };
}
