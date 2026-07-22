#pragma once

#include "../imgui_element.h"

namespace engine
{
    class DialogBox final : public ImGuiElement
    {
    public:
        DialogBox(const std::string& title)
            : ImGuiElement(Category::Window, title)
        {}

    protected:
        void draw() override
        {
            ImGui::Text("This is a dialog box");
            if (ImGui::Button("Close"))
                show(false);
        }
    };
}