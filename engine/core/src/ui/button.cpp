#include "../../include/ui/button.h"

void engine::UIButton::setup(GLFWwindow* window, const std::string& fontPath, int fontSize)
{
    m_window = window;
    m_rect.setup(window);
    m_text.setup(window, fontPath, fontSize);
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

void engine::UIButton::onClick(ClickCallback callback)
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
        m_borderThickness,
        m_borderRadius
    );


    // Text(centered)
    glm::vec2 textPos = getCenteredTextPosition();
    m_text.draw(m_label, textPos.x, textPos.y, 1.0f, m_textColor);

}

glm::vec2 engine::UIButton::getCenteredTextPosition() const
{
    glm::vec2 textSize = m_text.measure(m_label);

    return {
        m_position.x + (m_size.x - textSize.x) * 0.5f,
        m_position.y + (m_size.y - textSize.y) * 0.5f
    };
}

bool engine::UIButton::isMouseInside(double mouseX, double mouseY) const
{
    return mouseX >= m_position.x &&
        mouseX <= m_position.x + m_size.x &&
        mouseY >= m_position.y &&
        mouseY <= m_position.y + m_size.y;
}

void engine::UIButton::clean()
{
    
}