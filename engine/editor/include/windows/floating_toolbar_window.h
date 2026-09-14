#pragma once

#include <imgui.h>

#include "../imgui_element.h"

namespace engine
{
    class FloatingToolbarWindow final : public ImGuiElement
    {
    public:
        FloatingToolbarWindow() : ImGuiElement(Category::Window, "FloatingToolbar") {}

    private:
        void renderToolbar();

    protected:
        void draw() override
        {
            renderToolbar();
        }
    };
}