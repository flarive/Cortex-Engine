#pragma once

#include <imgui.h>

#include "imgui_element.h"

namespace engine
{
    class CustomWidget final : public ImGuiElement
    {
    public:
        CustomWidget() : ImGuiElement(Type::Widget, "ColorWidget") {}

    protected:
        void draw() override
        {
            ImGui::ColorEdit3("Tint", m_color);
        }

    private:
        float m_color[3] = { 1, 1, 1 };
    };
}