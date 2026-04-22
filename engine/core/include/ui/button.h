#pragma once

#include "ui.h"
#include "../../include/ui/rectangle.h"
#include <functional>

namespace engine
{
    class UIButton final : public UIBase
    {
    public:
        using ClickCallback = std::function<void()>;

        void setup(GLFWwindow* window);

        void update(double mouseX, double mouseY, bool mousePressed);
        void draw();

        void setPosition(glm::vec2 pos);
        void setSize(glm::vec2 size);

        void setColors(
            const Color& normal,
            const Color& hover,
            const Color& pressed,
            const Color& border
        );

        void setOnClick(ClickCallback callback);

        void clean() override;

    private:
        bool isMouseInside(double mouseX, double mouseY) const;

    private:
        GLFWwindow* m_window{ nullptr };

        UIRectangle m_rect;

        glm::vec2 m_position{ 0.0f };
        glm::vec2 m_size{ 100.0f, 40.0f };

        Color m_normalColor{ 0.2f, 0.2f, 0.2f, 1.0f };
        Color m_hoverColor{ 0.3f, 0.3f, 0.3f, 1.0f };
        Color m_pressedColor{ 0.1f, 0.1f, 0.1f, 1.0f };
        Color m_borderColor{ 1.0f, 1.0f, 1.0f, 1.0f };

        float m_borderThickness{ 0.02f };

        bool m_hovered{ false };
        bool m_pressed{ false };

        ClickCallback m_onClick;
    };
}
