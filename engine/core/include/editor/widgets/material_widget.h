#pragma once

#include "../imgui_element.h"

namespace engine
{
    class MaterialWidget final : public ImGuiElement
    {
    public:
        MaterialWidget() : ImGuiElement(Category::Widget, "ColorWidget") {}

        void init() override;

    protected:
        void draw() override;

    private:
        float m_color[3] = { 1, 1, 1 };
    };
}