#pragma once

#include "imgui_element.h"

#include "../common_defines.h"

namespace engine
{
    class PerformanceOverlay final : public ImGuiElement
    {
    public:
        PerformanceOverlay() : ImGuiElement(Category::Overlay, "PerformanceOverlay") {}

        void updatePerformanceCounters(const PerformanceCounters& counters);

    private:
        PerformanceCounters m_counters{};

    protected:
        void draw() override;
    };
}
