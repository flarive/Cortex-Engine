#pragma once

#include <imgui_internal.h>

#include "../imgui_element.h"

namespace engine
{
    class DockSpaceElement final : public ImGuiElement
    {
    public:
        DockSpaceElement() : ImGuiElement(Category::DockSpace, "DockSpace") {}
        ~DockSpaceElement() = default;

    protected:
        void begin() override;
        void draw(glm::mat4& projection, glm::mat4& view) override;
        void end() override;
    };
}