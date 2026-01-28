#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../common_defines.h"
#include "../texture.h"
#include "../shader.h"

namespace engine
{
    class SpriteRenderer
    {
    public:
        // Constructor (inits shaders/shapes)
        SpriteRenderer(Shader& shader);
        // Destructor
        ~SpriteRenderer();
        // Renders a defined quad textured with given sprite
        void DrawSprite(Texture& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
    private:
        // Render state
        Shader       shader;
        unsigned int quadVAO;
        // Initializes and configures the quad's buffer and vertex attributes
        void initRenderData();
    };
}