#include "../../../include/editor/overlays/performance_overlay.h"

#include <algorithm>


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
        pos.y -= ImGui::GetMainViewport()->Size.y / 3.0f;
        ImGui::SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        window_flags |= ImGuiWindowFlags_NoMove;
    }

    ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.85f);

    bool open = true; // ImGuiElement manages visibility itself
    if (ImGui::Begin("PerfDebugOverlay", &open, window_flags))
    {
        static ImVec4 GREY_DARK = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        static ImVec4 GREY_LIGHT = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
        
        static double accumulator = 0.0;
        static double lastTime = glfwGetTime();

        double now = glfwGetTime();
        double delta = now - lastTime;
        lastTime = now;

        accumulator += delta;

        auto vramInfo = m_vramManager.query();

        if (accumulator >= 1.0)   // sample every 1 second
        {
            m_sysMonitor.update();

            cachedCPU = m_sysMonitor.getCPU();
            cachedCPUProcess = m_sysMonitor.getCPUProcess();

            cachedRAMUsed = m_sysMonitor.getRAMUsed() / (1024.0 * 1024.0 * 1024.0); // GiB
            cachedRAMTotal = m_sysMonitor.getRAMTotal() / (1024.0 * 1024.0 * 1024.0); // GiB
            cachedProcessRAM = m_sysMonitor.getProcessRAM() / (1024.0 * 1024.0 * 1024.0); // GiB
            
            cachedRamPercent = (cachedRAMUsed / cachedRAMTotal) * 100.0;
            cachedRamProcessPercent = (cachedProcessRAM / cachedRAMTotal) * 100.0;

            cachedVramTotal = vramInfo.totalBytes / (1024.0 * 1024.0 * 1024.0); // GiB
            cachedVramUsed = vramInfo.usedBytes / (1024.0 * 1024.0 * 1024.0); // GiB
            cachedVramFree = vramInfo.freeBytes / (1024.0 * 1024.0 * 1024.0); // GiB
            cachedVramPercent = (cachedVramUsed / cachedVramTotal) * 100.0;

            cachedVendorGPUUsage = m_sysMonitor.getVendorGPUUsage();
            cachedVendorGPUUsagePercent = m_sysMonitor.getVendorGPUUsagePercent();
            cachedVendorTemperature = m_sysMonitor.getVendorTemperature();
            cachedVendorPowerUsageWatts = m_sysMonitor.getVendorPowerUsageWatts();


            accumulator = 0.0;
        }



        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));



        // ------------------------
        // ROW 1
        // -----------------------
        
        // FPS
        ImGui::BeginChild("FPS", ImVec2(160.0f, 38.0f), ImGuiChildFlags_None);
        {
            float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
            centerTextInBox("%.0f FPS", fps, std::nullopt, true, boxWidth, -4.0f, ImGui::Spectrum::fontLarge2, getFPSColor(fps));
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // Hardware
        ImGui::BeginChild("HARDWARE", ImVec2(240.0f, 38.0f), ImGuiChildFlags_None);
        {
            float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
            centerTextInBox(m_sysMonitor.GetGPURenderer(), std::nullopt, std::nullopt, false, boxWidth, 8.0f, ImGui::Spectrum::fontSmall1);
        }
        ImGui::EndChild();

        // ------------------------
        // ROW 2
        // -----------------------

        // CPU
        ImGui::BeginChild("CPU_2", ImVec2(100.0f, 70.0f), ImGuiChildFlags_None);
        {
            float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
            centerTextInBox("CPU", std::nullopt, std::nullopt, true, boxWidth, 0.0f, ImGui::Spectrum::fontMedium2, GREY_DARK);

            ImGui::BeginChild("CPU1", ImVec2(50.0f, 0.0f), ImGuiChildFlags_None);
            {
                float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
                centerTextInBox("%.0f%%", std::floor(cachedCPU), std::nullopt, true, boxWidth, -6.0f, ImGui::Spectrum::fontMedium2);
                centerTextInBox("TOTAL", std::nullopt, std::nullopt, true, boxWidth, 12.0f, ImGui::Spectrum::fontSmall1, GREY_DARK);
            }
            ImGui::EndChild();

            ImGui::SameLine();

            ImGui::BeginChild("CPU2", ImVec2(50.0f, 0.0f), ImGuiChildFlags_None);
            {
                float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
                centerTextInBox("%.0f%%", std::floor(cachedCPUProcess), std::nullopt, true, boxWidth, -6.0f, ImGui::Spectrum::fontMedium2);
                centerTextInBox("APP", std::nullopt, std::nullopt, true, boxWidth, 12.0f, ImGui::Spectrum::fontSmall1, GREY_DARK);
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // RAM
        ImGui::BeginChild("RAM_2", ImVec2(100.0f, 70.0f), ImGuiChildFlags_None);
        {
            float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
            centerTextInBox("RAM", std::nullopt, std::nullopt, true, boxWidth, 0.0f, ImGui::Spectrum::fontMedium2, GREY_DARK);

            ImGui::BeginChild("RAM1", ImVec2(50.0f, 0.0f), ImGuiChildFlags_None);
            {
                float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
                centerTextInBox("%.0f%%", cachedRamPercent, std::nullopt, true, boxWidth, -6.0f, ImGui::Spectrum::fontMedium2);
                centerTextInBox("TOTAL", std::nullopt, std::nullopt, true, boxWidth, 12.0f, ImGui::Spectrum::fontSmall1, GREY_DARK);
            }
            ImGui::EndChild();

            ImGui::SameLine();

            ImGui::BeginChild("RAM2", ImVec2(50.0f, 0.0f), ImGuiChildFlags_None);
            {
                float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
                centerTextInBox("%.0f%%", cachedRamProcessPercent, std::nullopt, true, boxWidth, -6.0f, ImGui::Spectrum::fontMedium2);
                centerTextInBox("APP", std::nullopt, std::nullopt, true, boxWidth, 12.0f, ImGui::Spectrum::fontSmall1, GREY_DARK);
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // GPU
        ImGui::BeginChild("GPU_2", ImVec2(100.0f, 70.0f), ImGuiChildFlags_None);
        {
            float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
            centerTextInBox("GPU", std::nullopt, std::nullopt, true, boxWidth, 0.0f, ImGui::Spectrum::fontMedium2, GREY_DARK);

            ImGui::BeginChild("GPU1", ImVec2(100.0f, 0.0f), ImGuiChildFlags_None);
            {
                float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
                centerTextInBox("%.0f%%", cachedVendorGPUUsagePercent, std::nullopt, true, boxWidth, -6.0f, ImGui::Spectrum::fontMedium2);
                centerTextInBox("TOTAL", std::nullopt, std::nullopt, true, boxWidth, 12.0f, ImGui::Spectrum::fontSmall1, GREY_DARK);
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // VRAM
        ImGui::BeginChild("VRAM_2", ImVec2(100.0f, 70.0f), ImGuiChildFlags_None);
        {
            float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
            centerTextInBox("VRAM", std::nullopt, std::nullopt, true, boxWidth, 0.0f, ImGui::Spectrum::fontMedium2, GREY_DARK);

            ImGui::BeginChild("VRAM1", ImVec2(100.0f, 0.0f), ImGuiChildFlags_None);
            {
                float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
                centerTextInBox("%.0f%%", cachedVramPercent, std::nullopt, true, boxWidth, -6.0f, ImGui::Spectrum::fontMedium2);
                centerTextInBox("TOTAL", std::nullopt, std::nullopt, true, boxWidth, 12.0f, ImGui::Spectrum::fontSmall1, GREY_DARK);
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();


        // ------------------------
        // ROW 3
        // -----------------------

        // CPU
        ImGui::BeginChild("CPU_3", ImVec2(100.0f, 35.0f), ImGuiChildFlags_None);
        {
            float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
            centerTextInBox("TIME / FRAME", std::nullopt, std::nullopt, true, boxWidth, 0.0f, ImGui::Spectrum::fontSmall1, GREY_DARK);
            centerTextInBox("%.0f ms", cpuTime, std::nullopt, true, boxWidth, 15.0f, ImGui::Spectrum::fontSmall1);
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // RAM
        ImGui::BeginChild("RAM_3", ImVec2(100.0f, 35.0f), ImGuiChildFlags_None);
        {
            float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
            centerTextInBox("USED", std::nullopt, std::nullopt, true, boxWidth, 0.0f, ImGui::Spectrum::fontSmall1, GREY_DARK);
            centerTextInBox("%.2f / %.1f Gb", cachedRAMUsed, cachedRAMTotal, true, boxWidth, 15.0f, ImGui::Spectrum::fontSmall1);
            
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // GPU
        ImGui::BeginChild("GPU_3", ImVec2(100.0f, 35.0f), ImGuiChildFlags_None);
        {
            float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
            centerTextInBox("TIME / FRAME", std::nullopt, std::nullopt, true, boxWidth, 0.0f, ImGui::Spectrum::fontSmall1, GREY_DARK);
            centerTextInBox("%.0f ms", gpuTime, std::nullopt, true, boxWidth, 15.0f, ImGui::Spectrum::fontSmall1);
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // VRAM
        ImGui::BeginChild("VRAM_3", ImVec2(100.0f, 35.0f), ImGuiChildFlags_None);
        {
            float boxWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemSpacing.x;
            centerTextInBox("USED", std::nullopt, std::nullopt, true, boxWidth, 0.0f, ImGui::Spectrum::fontSmall1, GREY_DARK);
            centerTextInBox("%.2f / %.1f Gb", cachedVramUsed, cachedVramTotal, true, boxWidth, 15.0f, ImGui::Spectrum::fontSmall1);
        }
        ImGui::EndChild();


        ImGui::PopStyleVar(4);


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

void engine::PerformanceOverlay::centerTextInBox(const std::string& header, std::optional<double> value1, std::optional<double> value2, bool offsetX, float boxWidth, float offsetY, ImFont* font, const ImVec4& color)
{
    char buf[64];

    if (value1.has_value() && value2.has_value())
    {
        if (header.find('%') != std::string::npos)
            snprintf(buf, sizeof(buf), header.c_str(), *value1, *value2);
        else
            snprintf(buf, sizeof(buf), "%s %.0f %.0f", header.c_str(), *value1, *value2);
    }
    else if (value1.has_value())
    {
        if (header.find('%') != std::string::npos)
            snprintf(buf, sizeof(buf), header.c_str(), *value1);
        else
            snprintf(buf, sizeof(buf), "%s %.0f", header.c_str(), *value1);
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

    ImGui::SetCursorPosY(offsetY);

    ImGui::TextColored(color, "%s", buf);

    ImGui::PopFont();
}

ImVec4 engine::PerformanceOverlay::getFPSColor(float fps)
{
    const ImVec4 red = ImVec4(1.0f, 0.1f, 0.1f, 1.0f);
    const ImVec4 orange = ImVec4(1.0f, 0.5f, 0.1f, 1.0f);
    const ImVec4 green = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);

    if (fps < 30.0f)
        return red;

    if (fps < 45.0f)
    {
        float t = (fps - 30.0f) / (45.0f - 30.0f);   // 0 => 1
        return lerpColor(red, orange, t);
    }

    if (fps < 60.0f)
    {
        float t = (fps - 45.0f) / (60.0f - 45.0f);   // 0 => 1
        return lerpColor(orange, green, t);
    }

    return green;
}

ImVec4 engine::PerformanceOverlay::getPercentColor(float percent)
{
    const ImVec4 red = ImVec4(1.0f, 0.1f, 0.1f, 1.0f);
    const ImVec4 orange = ImVec4(1.0f, 0.5f, 0.1f, 1.0f);
    const ImVec4 green = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);

    if (percent < 30.0f)
        return red;

    if (percent < 45.0f)
    {
        float t = (percent - 30.0f) / (45.0f - 30.0f);   // 0 => 1
        return lerpColor(red, orange, t);
    }

    if (percent < 60.0f)
    {
        float t = (percent - 45.0f) / (60.0f - 45.0f);   // 0 => 1
        return lerpColor(orange, green, t);
    }

    return green;
}

ImVec4 engine::PerformanceOverlay::lerpColor(const ImVec4& a, const ImVec4& b, float t)
{
    return ImVec4(
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t,
        a.w + (b.w - a.w) * t
    );
}

