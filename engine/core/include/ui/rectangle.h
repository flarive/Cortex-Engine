#pragma once

#include "ui.h"

namespace engine
{
    class UIRectangle final : public UIBase
    {
    public:
        unsigned int width{};
        unsigned int height{};

        UIRectangle();
        ~UIRectangle() override;

        void setup(GLFWwindow* window);
        void draw(glm::vec2 position, glm::vec2 size, float rotate, const Color& fillColor, const Color& borderColor, float borderThickness = 0.02f, float borderRadius = 6.0f);

        const Shader& getRectShader() { return m_rectShader; }

        void clean() override;

    private:
        static Shader m_rectShader;

        void initRenderData();
    };
}