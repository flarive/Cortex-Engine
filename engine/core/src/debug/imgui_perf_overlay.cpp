#include "../../include/debug/imgui_perf_overlay.h"

#include <imgui_internal.h>

void engine::ImGuiPerfOverlay::renderPerfOverlay(bool* p_open, const float& fps, const double& cpuTime, const double& gpuTime, const double& uiTime)
{
    static int location = -2;
    ImGuiIO& io = ImGui::GetIO();
    static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (location >= 0)
    {
        const float PAD = 10.0f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowViewport(viewport->ID);
        window_flags |= ImGuiWindowFlags_NoMove;
    }
    else if (location == -2)
    {
        // Center window
        ImVec2 zzz = ImGui::GetMainViewport()->GetCenter();
        zzz.y = 240.0f;
        ImGui::SetNextWindowPos(zzz, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        window_flags |= ImGuiWindowFlags_NoMove;
    }

    ImGui::SetNextWindowSize(ImVec2(300.0f, 0.0f), ImGuiCond_Always); // fixed width
    ImGui::SetNextWindowBgAlpha(0.85f); // Transparent background
    if (ImGui::Begin("PerfDebugOverlay", p_open, window_flags))
    {
        ImGui::Text("FPS: %.2f", fps);
        ImGui::Text("CPU time: %.2f ms", cpuTime);
        ImGui::Text("GPU time: %.2f ms", gpuTime);
        ImGui::Text("UI time: %.2f ms", uiTime);

        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Custom", NULL, location == -1)) location = -1;
            if (ImGui::MenuItem("Center", NULL, location == -2)) location = -2;
            if (ImGui::MenuItem("Top-left", NULL, location == 0)) location = 0;
            if (ImGui::MenuItem("Top-right", NULL, location == 1)) location = 1;
            if (ImGui::MenuItem("Bottom-left", NULL, location == 2)) location = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, location == 3)) location = 3;
            if (p_open && ImGui::MenuItem("Close")) *p_open = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}
