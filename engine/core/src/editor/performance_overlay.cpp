#include "../../include/editor/performance_overlay.h"

void engine::PerformanceOverlay::init()
{
    if (!m_initDone)
    {
        m_vramManager.init();
        m_initDone = true;
    }
}

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
        char buf[64];
        snprintf(buf, sizeof(buf), "FPS: %.0f", fps);


        ImGui::PushFont(ImGui::Spectrum::fontLarge);

        ImVec2 size = ImGui::CalcTextSize(buf);
        float center = (ImGui::GetWindowWidth() - size.x) * 0.5f;

        ImGui::SetCursorPosX(center);
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", buf);

        ImGui::PopFont();



        ImGui::PushFont(ImGui::Spectrum::fontSmall);
        ImGui::TextColored(ImVec4(1, 1, 0.2f, 1), "Frame duration: %.2f ms", deltaTime * 1000.0f);
        ImGui::PopFont();


        ImGui::PushFont(ImGui::Spectrum::fontMedium);
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 1, 1), "CPU time per frame: %.2f ms", cpuTime);
        ImGui::PopFont();

        ImGui::PushFont(ImGui::Spectrum::fontMedium);
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 1, 1), "GPU time per frame: %.2f ms", gpuTime);
        ImGui::PopFont();

        ImGui::PushFont(ImGui::Spectrum::fontMedium);
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 1, 1), "UI time per frame: %.2f ms", uiTime);
        ImGui::PopFont();


        

        
        





        // --- SAMPLE EVERY 1 s ---
        static double accumulator = 0.0;
        static double lastTime = glfwGetTime();

        double now = glfwGetTime();
        double delta = now - lastTime;
        lastTime = now;

        accumulator += delta;

        if (accumulator >= 1.0)   // sample every 1 second
        {
            m_sysMonitor.update();

            cachedCPU = m_sysMonitor.getCPU();

            cachedCPUProcess = m_sysMonitor.getCPUProcess();
            cachedRAMUsed = m_sysMonitor.getRAMUsed();
            cachedRAMTotal = m_sysMonitor.getRAMTotal();
            cachedProcessRAM = m_sysMonitor.getProcessRAM();

            accumulator = 0.0;
        }



        ImGui::Text("GPU Vendor:\n%s", m_sysMonitor.GetGPUVendor().c_str());
        ImGui::Text("GPU Renderer:\n%s", m_sysMonitor.GetGPURenderer().c_str());
        ImGui::Text("OpenGL Version:\n%s", m_sysMonitor.GetGPUVersion().c_str());
        ImGui::Text(" ");

        ImGui::Text("Application average %.3f ms\nFrame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);


        auto vramInfo = m_vramManager.query();

        double total = vramInfo.totalBytes / (1024.0 * 1024.0);
        ImGui::Text("GPU VRAM Total: %.0f MB", total);

        double used = vramInfo.usedBytes / (1024.0 * 1024.0);
        double free = vramInfo.freeBytes / (1024.0 * 1024.0);
        ImGui::Text("GPU VRAM Used: %.0f MB / Free: %.0f MB", used, free);




        ImGui::Text("CPU total : %.0f %%", cachedCPU);
        ImGui::Text("CPU app : %.0f %%", cachedCPUProcess);


        ImGui::Text("RAM total : %.2f / %.2f GB", cachedRAMUsed / (1024.0 * 1024 * 1024), cachedRAMTotal / (1024.0 * 1024 * 1024));
        ImGui::Text("RAM app : %.2f MB", cachedProcessRAM / (1024.0 * 1024.0));



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

    ImGui::End();
}

void engine::PerformanceOverlay::centerTextInBox(const std::string& header, float value, float boxWidth, ImFont* font)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "FPS %.0f", value);

    ImGui::PushFont(font);

    ImVec2 size = ImGui::CalcTextSize(buf);
    float center = (boxWidth - size.x) * 0.5f;

    ImGui::SetCursorPosX(center);
    ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", buf);

    ImGui::PopFont();


    //char buf[64];
    //snprintf(buf, sizeof(buf), "FPS: %.0f", value);


    //ImGui::PushFont(ImGui::Spectrum::fontLarge);

    //ImVec2 size = ImGui::CalcTextSize(buf);
    //float center = (ImGui::GetWindowWidth() - size.x) * 0.5f;

    //ImGui::SetCursorPosX(center);
    //ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", buf);

    //ImGui::PopFont();
}
