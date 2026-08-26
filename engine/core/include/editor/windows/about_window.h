#pragma once

#include "../imgui_element.h"

#include "../../tools/system_monitor.h"
#include "../../tools/vram_manager.h"

#include "../../../include/managers/log_manager.h"

namespace engine
{
    class AboutWindow final : public ImGuiElement
    {
    public:
        AboutWindow();
        ~AboutWindow();

        void init() override;

    private:
        SystemMonitor m_sysMonitor{};
        VramManager m_vramManager{};


        double cachedCPU = 0.0;
        double cachedCPUProcess = 0.0;

        uint64_t cachedRAMUsed = 0;
        uint64_t cachedRAMTotal = 0;
        uint64_t cachedProcessRAM = 0;

        void renderTabAbout();

    protected:
        void draw() override
        {
            renderTabAbout();
        }
    };
}