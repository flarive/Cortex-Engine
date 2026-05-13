#pragma once

#include "ui.h"
#include "../../include/ui/rectangle.h"
#include "../../include/ui/text.h"
#include <functional>

namespace engine
{
    class UIButton final : public UIBase
    {
    public:
        
        using ClickCallback = std::function<void()>;

        UIButton();
        ~UIButton() override;


        void setup(GLFWwindow* window, const std::string& fontPath, int fontSize);

        void update(double mouseX, double mouseY, bool mousePressed);
        void draw();

        void setPosition(glm::vec2 pos);
        void setSize(glm::vec2 size);

        void setText(const std::string& text) { m_label = text; }
        void setTextColor(const Color& color) { m_textColor = color; }

        void setBorderThickness(float thickness) { m_borderThickness = thickness; }
        void setBorderRadius(float radius) { m_borderRadius = radius; }

        void setColors(
            const Color& normal,
            const Color& hover,
            const Color& pressed,
            const Color& border
        );

        void onClick(ClickCallback callback);

        void clean() override;

    private:
        bool isMouseInside(double mouseX, double mouseY) const;

    private:
        GLFWwindow* m_window{ nullptr };

        UIRectangle m_rect;
        UIText m_text;

        glm::vec2 m_position{ 0.0f };
        glm::vec2 m_size{ 100.0f, 40.0f };


        std::string m_label{ "Button" };
        Color m_textColor{ 1, 1, 1, 1 };

        Color m_normalColor{ 0.2f, 0.2f, 0.2f, 1.0f };
        Color m_hoverColor{ 0.3f, 0.3f, 0.3f, 1.0f };
        Color m_pressedColor{ 0.1f, 0.1f, 0.1f, 1.0f };
        Color m_borderColor{ 1.0f, 1.0f, 1.0f, 1.0f };

        float m_borderThickness{ 0.02f };
        float m_borderRadius{ 6.0f };

        bool m_hovered{ false };
        bool m_pressed{ false };

        ClickCallback m_onClick;

        glm::vec2 getCenteredTextPosition() const;
    };
}