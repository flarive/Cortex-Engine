#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"
#include "../misc/colors.h"

#include "../shader.h"

#include <glm/glm.hpp>

namespace engine
{
    class UIRectangle final : private NonCopyable
    {
    public:
        unsigned int width{};
        unsigned int height{};

        // Constructor (inits shaders/shapes)
        UIRectangle() = default;
        // Destructor
        ~UIRectangle();

        void setup(GLFWwindow* window);
        void draw(glm::vec2 position, glm::vec2 size, float rotate, const Color& fillColor, const Color& borderColor, float borderThickness = 0.02f);

        void clean();

    private:
        GLFWwindow* m_window{};
        unsigned int m_quadVAO{};

        static Shader m_rectShader;

        void initRenderData();
    };
}