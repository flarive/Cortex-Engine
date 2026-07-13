#pragma once

#include "imgui_element.h"

#include "../tools/system_monitor.h"
#include "../tools/cpu_usage.h"
#include "../tools/vram_manager.h"


namespace engine
{
    class AboutWindow final : public ImGuiElement
    {
    public:
        AboutWindow() : ImGuiElement(Category::Window, "About") {}

        void init();

    private:
        SystemMonitor m_sysMonitor{};
        VramManager m_vramManager{};

        CpuUsage m_cpuUsage{};
        
        void renderTabAbout();

        

    protected:
        void draw() override
        {
            renderTabAbout();
        }
    };
}