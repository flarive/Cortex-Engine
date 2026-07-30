#pragma once

#include "../imgui_element.h"

#include "../../common_defines.h"
#include "../../tools/system_monitor.h"
#include "../../tools/vram_manager.h"

#include <optional>

namespace engine
{
    class PerformanceOverlay final : public ImGuiElement
    {
    public:
        PerformanceOverlay() : ImGuiElement(Category::Overlay, "PerformanceOverlay") {}

        void init() override;
        void updatePerformanceCounters(const PerformanceCounters& counters);

    private:
        bool m_initDone{ false };
        
        PerformanceCounters m_counters{};

        SystemMonitor m_sysMonitor{};
        VramManager m_vramManager{};


        double cachedCPU = 0.0;
        double cachedCPUProcess = 0.0;

        double cachedRAMUsed = 0;
        double cachedRAMTotal = 0;
        double cachedProcessRAM = 0;
        double cachedRamPercent = 0;
        double cachedRamProcessPercent = 0;


        double cachedVramTotal = 0;
        double cachedVramUsed = 0;
        double cachedVramFree = 0;
        double cachedVramPercent = 0;


		int cachedVendorGPUUsage = 0;
		double cachedVendorGPUUsagePercent = 0.0;
		int cachedVendorTemperature = 0;
		double cachedVendorPowerUsageWatts = 0.0;



        void centerTextInBox(const std::string& header, std::optional<double> value1, std::optional<double> value2, bool offsetX, float boxWidth, float offsetY, ImFont* font = ImGui::Spectrum::fontSmall1, const ImVec4& color = ImVec4(1, 1, 1, 1));

        ImVec4 getFPSColor(float fps);
        ImVec4 getPercentColor(float percent);
        static ImVec4 lerpColor(const ImVec4& a, const ImVec4& b, float t);

    protected:
        void draw() override;
    };
}
