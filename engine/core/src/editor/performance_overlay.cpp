#include "../../include/editor/performance_overlay.h"

void engine::PerformanceOverlay::updatePerformanceCounters(const PerformanceCounters& counters)
{
    m_counters = counters;
}
    
void engine::PerformanceOverlay::draw()
{
    // You probably want these values to come from your engine profiler.
    // For now, I keep them static placeholders.
    const float fps = m_counters.fps;
    const float deltaTime = m_counters.deltaTime;
    const double cpuTime = m_counters.cpuTime;
    const double gpuTime = m_counters.gpuTime;
    const double uiTime = m_counters.uiTime;

    static int location = -2;
    ImGuiIO& io = ImGui::GetIO();

    static ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    // Handle overlay positioning
    if (location >= 0)
    {
        const float PAD = 10.0f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos;
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
        ImVec2 pos = ImGui::GetMainViewport()->GetCenter();
        pos.y -= ImGui::GetMainViewport()->Size.y / 2.5f;
        ImGui::SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        window_flags |= ImGuiWindowFlags_NoMove;
    }

    ImGui::SetNextWindowSize(ImVec2(300.0f, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.85f);

    bool open = true; // ImGuiElement manages visibility itself
    if (ImGui::Begin("PerfDebugOverlay", &open, window_flags))
    {
        ImGui::Text("FPS: %.2f", fps);
        ImGui::Text("CPU time: %.2f ms", cpuTime);
        ImGui::Text("GPU time: %.2f ms", gpuTime);
        ImGui::Text("UI time: %.2f ms", uiTime);
        ImGui::Text("Delta time: %.2f ms", deltaTime);

        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Custom", nullptr, location == -1)) location = -1;
            if (ImGui::MenuItem("Center", nullptr, location == -2)) location = -2;
            if (ImGui::MenuItem("Top-left", nullptr, location == 0)) location = 0;
            if (ImGui::MenuItem("Top-right", nullptr, location == 1)) location = 1;
            if (ImGui::MenuItem("Bottom-left", nullptr, location == 2)) location = 2;
            if (ImGui::MenuItem("Bottom-right", nullptr, location == 3)) location = 3;

            if (ImGui::MenuItem("Close"))
                show(false);

            ImGui::EndPopup();
        }
    }

    //ImGui::PushFont(ImGui::Spectrum::fontLarge);
    //ImGui::TextColored(ImVec4(1, 0.2f, 0.2f, 1), "LARGE RED TITLE");
    //ImGui::PopFont();
    //    
    //ImGui::PushFont(ImGui::Spectrum::fontMedium);
    //ImGui::TextColored(ImVec4(0.2f, 0.8f, 1, 1), "Medium cyan text");
    //ImGui::PopFont();
    //    
    //ImGui::PushFont(ImGui::Spectrum::fontSmall);
    //ImGui::TextColored(ImVec4(1, 1, 0.2f, 1), "Small yellow text");
    //ImGui::PopFont();

    ImGui::End();
}
