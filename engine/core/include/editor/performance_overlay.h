#pragma once

#include "imgui_element.h"

#include "../common_defines.h"
#include "../tools/system_monitor.h"
#include "../tools/vram_manager.h"

#include <optional>

namespace engine
{
    class PerformanceOverlay final : public ImGuiElement
    {
    public:
        PerformanceOverlay() : ImGuiElement(Category::Overlay, "PerformanceOverlay") {}

        void init();
        void updatePerformanceCounters(const PerformanceCounters& counters);

    private:
        bool m_initDone{ false };
        
        PerformanceCounters m_counters{};

        SystemMonitor m_sysMonitor{};
        VramManager m_vramManager{};


        double cachedCPU = 0.0;
        double cachedCPUProcess = 0.0;

        uint64_t cachedRAMUsed = 0;
        uint64_t cachedRAMTotal = 0;
        uint64_t cachedProcessRAM = 0;

        double cachedRamPercent = 0;
        double cachedRamProcessPercent = 0;

        void centerTextInBox(const std::string& header, std::optional<double> value, bool offsetX, float boxWidth, float yOffset, ImFont* font = ImGui::Spectrum::fontSmall1, const ImVec4& color = ImVec4(1, 1, 1, 1));

    protected:
        void draw() override;
    };
}
