#pragma once

#include "imgui_element.h"

namespace engine
{
    class DebugOverlay final : public ImGuiElement
    {
    public:
        DebugOverlay() : ImGuiElement(Category::Overlay, "DebugOverlay") {}

    protected:
        void draw() override
        {
            ImGui::TextColored(ImVec4(1, 0.2f, 0.2f, 1), "FPS: %.2f", ImGui::GetIO().Framerate);
            ImGui::Text("Camera Pos: (%.2f, %.2f, %.2f)", camX, camY, camZ);
        }

        float camX = 0, camY = 0, camZ = 0;
    };
}
