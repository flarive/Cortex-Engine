#pragma once

#include <imgui_internal.h>

#include "imgui_element.h"

namespace engine
{
    class DockSpaceElement final : public ImGuiElement
    {
    public:
        DockSpaceElement() : ImGuiElement(Category::DockSpace, "DockSpace") {}

    protected:
        void begin() override;
        void draw() override;
        void end() override;
    };
}