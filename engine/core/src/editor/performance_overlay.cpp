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

    static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus;

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

    ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.85f);

    bool open = true; // ImGuiElement manages visibility itself
    if (ImGui::Begin("PerfDebugOverlay", &open, window_flags))
    {
        static ImVec4 GREY = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        
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
            cachedRAMUsed = m_sysMonitor.getRAMUsed() / (1024.0 * 1024 * 1024);
            cachedRAMTotal = m_sysMonitor.getRAMTotal() / (1024.0 * 1024 * 1024);
            cachedProcessRAM = m_sysMonitor.getProcessRAM() / (1024.0 * 1024.0);
            
            cachedRamPercent = (cachedRAMUsed / cachedRAMTotal) * 100.0;
            cachedRamProcessPercent = (cachedProcessRAM / cachedRAMTotal) * 100.0;

            accumulator = 0.0;
        }


        

        // FPS
        //ImGui::BeginChild("LeftBox", ImVec2(150.0f, 40.0f), ImGuiChildFlags_None);
        //{
        //    float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
        //    centerTextInBox("FPS: %.0f", fps, true, boxWidth, 0.0f, ImGui::Spectrum::fontLarge);
        //}
        //ImGui::EndChild();

        //ImGui::SameLine();

        //// Hardware
        //ImGui::BeginChild("RightBox", ImVec2(150.0f, 40.0f), ImGuiChildFlags_None);
        //{
        //    float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
        //    centerTextInBox("FPS: %.0f", fps, true, boxWidth, 0.0f, ImGui::Spectrum::fontSmall);
        //}
        //ImGui::EndChild();


        // CPU
        ImGui::BeginChild("CPU", ImVec2(100.0f, 90.0f), ImGuiChildFlags_None);
        {
            float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
            centerTextInBox("CPU", std::nullopt, true, boxWidth, 0.0f, ImGui::Spectrum::fontLarge1, GREY);

            ImGui::BeginChild("CPU1", ImVec2(40.0f, 0.0f), ImGuiChildFlags_None);
            {
                float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
                centerTextInBox("%.0f%%", cachedCPU, true, boxWidth, 0.0f, ImGui::Spectrum::fontMedium2);
                centerTextInBox("TOTAL", std::nullopt, true, boxWidth, 25.0f, ImGui::Spectrum::fontSmall1, GREY);
            }
            ImGui::EndChild();

            ImGui::SameLine();

            ImGui::BeginChild("CPU2", ImVec2(40.0f, 0.0f), ImGuiChildFlags_None);
            {
                float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
                centerTextInBox("%.0f%%", cachedCPUProcess, true, boxWidth, 0.0f, ImGui::Spectrum::fontMedium2);
                centerTextInBox("APP", std::nullopt, true, boxWidth, 25.0f, ImGui::Spectrum::fontSmall1, GREY);
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // RAM
        ImGui::BeginChild("RAM", ImVec2(100.0f, 40.0f), ImGuiChildFlags_None);
        {
            float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
            centerTextInBox("RAM", std::nullopt, true, boxWidth, 0.0f, ImGui::Spectrum::fontLarge1, GREY);

            ImGui::BeginChild("RAM1", ImVec2(40.0f, 0.0f), ImGuiChildFlags_None);
            {
                float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
                centerTextInBox("%.0f%%", cachedRamPercent, true, boxWidth, 0.0f, ImGui::Spectrum::fontMedium2);
                centerTextInBox("TOTAL", std::nullopt, true, boxWidth, 25.0f, ImGui::Spectrum::fontSmall1, GREY);
            }
            ImGui::EndChild();

            ImGui::SameLine();

            ImGui::BeginChild("RAM2", ImVec2(40.0f, 0.0f), ImGuiChildFlags_None);
            {
                float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
                centerTextInBox("%.0f%%", cachedRamProcessPercent, true, boxWidth, 0.0f, ImGui::Spectrum::fontMedium2);
                centerTextInBox("APP", std::nullopt, true, boxWidth, 25.0f, ImGui::Spectrum::fontSmall1, GREY);
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // GPU
        ImGui::BeginChild("GPU", ImVec2(100.0f, 40.0f), ImGuiChildFlags_None);
        {
            float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
            centerTextInBox("GPU", std::nullopt, true, boxWidth, 0.0f, ImGui::Spectrum::fontLarge1, GREY);
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // VRAM
        ImGui::BeginChild("VRAM", ImVec2(100.0f, 40.0f), ImGuiChildFlags_None);
        {
            float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
            centerTextInBox("VRAM", std::nullopt, true, boxWidth, 0.0f, ImGui::Spectrum::fontLarge1, GREY);
        }
        ImGui::EndChild();


        /*ImGui::PushFont(ImGui::Spectrum::fontSmall);
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
        ImGui::PopFont();*/


        

        
        







        //ImGui::Text("GPU Vendor:\n%s", m_sysMonitor.GetGPUVendor().c_str());
        //ImGui::Text("GPU Renderer:\n%s", m_sysMonitor.GetGPURenderer().c_str());
        //ImGui::Text("OpenGL Version:\n%s", m_sysMonitor.GetGPUVersion().c_str());
        //ImGui::Text(" ");

        //ImGui::Text("Application average %.3f ms\nFrame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);


        //auto vramInfo = m_vramManager.query();

        //double total = vramInfo.totalBytes / (1024.0 * 1024.0);
        //ImGui::Text("GPU VRAM Total: %.0f MB", total);

        //double used = vramInfo.usedBytes / (1024.0 * 1024.0);
        //double free = vramInfo.freeBytes / (1024.0 * 1024.0);
        //ImGui::Text("GPU VRAM Used: %.0f MB / Free: %.0f MB", used, free);




        //ImGui::Text("CPU total : %.0f %%", cachedCPU);
        //ImGui::Text("CPU app : %.0f %%", cachedCPUProcess);


        //ImGui::Text("RAM total : %.2f / %.2f GB", cachedRAMUsed / (1024.0 * 1024 * 1024), cachedRAMTotal / (1024.0 * 1024 * 1024));
        //ImGui::Text("RAM app : %.2f MB", cachedProcessRAM / (1024.0 * 1024.0));



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

void engine::PerformanceOverlay::centerTextInBox(const std::string& header, std::optional<double> value, bool offsetX, float boxWidth, float yOffset, ImFont* font, const ImVec4& color)
{
    char buf[64];

    if (value.has_value())
    {
        if (header.find('%') != std::string::npos)
            snprintf(buf, sizeof(buf), header.c_str(), *value);
        else
            snprintf(buf, sizeof(buf), "%s %.0f", header.c_str(), *value);
    }
    else
    {
        // No value just print the header
        snprintf(buf, sizeof(buf), "%s", header.c_str());
    }


    ImGui::PushFont(font);

    ImVec2 size = ImGui::CalcTextSize(buf);
    float center = (boxWidth - size.x) * 0.5f;

	if (offsetX)
		ImGui::SetCursorPosX(center);

    ImGui::SetCursorPosY(yOffset);

    ImGui::TextColored(color, "%s", buf);

    ImGui::PopFont();
}
