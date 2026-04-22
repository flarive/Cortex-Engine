#include "../../include/ui/button.h"

void engine::UIButton::setup(GLFWwindow* window)
{
    m_window = window;
    m_rect.setup(window);
}

void engine::UIButton::setPosition(glm::vec2 pos)
{
    m_position = pos;
}

void engine::UIButton::setSize(glm::vec2 size)
{
    m_size = size;
}

void engine::UIButton::setColors(
    const Color& normal,
    const Color& hover,
    const Color& pressed,
    const Color& border)
{
    m_normalColor = normal;
    m_hoverColor = hover;
    m_pressedColor = pressed;
    m_borderColor = border;
}

void engine::UIButton::setOnClick(ClickCallback callback)
{
    m_onClick = callback;
}

void engine::UIButton::update(double mouseX, double mouseY, bool mousePressed)
{
    m_hovered = isMouseInside(mouseX, mouseY);

    if (m_hovered && mousePressed && !m_pressed)
    {
        // mouse button pressed inside
        m_pressed = true;
    }

    if (m_pressed && !mousePressed)
    {
        // mouse button released
        m_pressed = false;

        if (m_hovered && m_onClick)
            m_onClick();
    }
}

void engine::UIButton::draw()
{
    const Color* currentColor = &m_normalColor;

    if (m_pressed)
        currentColor = &m_pressedColor;
    else if (m_hovered)
        currentColor = &m_hoverColor;

    m_rect.draw(
        m_position,
        m_size,
        0.0f,
        *currentColor,
        m_borderColor,
        m_borderThickness
    );
}

bool engine::UIButton::isMouseInside(double mouseX, double mouseY) const
{
    // GLFW mouse Y is from top → flip if your UI origin is bottom-left
    int winW, winH;
    glfwGetWindowSize(m_window, &winW, &winH);
    mouseY = winH - mouseY;

    return mouseX >= m_position.x &&
        mouseX <= m_position.x + m_size.x &&
        mouseY >= m_position.y &&
        mouseY <= m_position.y + m_size.y;
}

void engine::UIButton::clean()
{
    
}