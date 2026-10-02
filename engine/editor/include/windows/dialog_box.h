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
        ~DialogBox() = default;

    protected:
        void draw(glm::mat4& projection, glm::mat4& view) override
        {
            ImGui::Text("This is a dialog box");
            if (ImGui::Button("Close"))
                show(false);
        }
    };
}