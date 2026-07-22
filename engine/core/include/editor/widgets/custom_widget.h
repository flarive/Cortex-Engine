#pragma once

#include "../imgui_element.h"

namespace engine
{
    class CustomWidget final : public ImGuiElement
    {
    public:
        CustomWidget() : ImGuiElement(Category::Widget, "ColorWidget") {}

    protected:
        void draw() override
        {
            ImGui::ColorEdit3("Tint", m_color);
        }

    private:
        float m_color[3] = { 1, 1, 1 };
    };
}