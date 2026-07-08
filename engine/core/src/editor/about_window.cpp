#include "../../include/editor/about_window.h"


void engine::AboutWindow::init()
{
    m_vramManager.init();
}

void engine::AboutWindow::renderTabAbout()
{
    auto vramInfo = m_vramManager.query();

    ImGui::BeginChild("AboutRegion", ImVec2(0, 0), true);
    ImGui::Text("GPU Vendor:\n%s", m_sysMonitor.GetGPUVendor().c_str());

    double total = vramInfo.totalBytes / (1024.0 * 1024.0);
    ImGui::Text("GPU VRAM Total: %.0f MB", total);

    double used = vramInfo.usedBytes / (1024.0 * 1024.0);
    double free = vramInfo.freeBytes / (1024.0 * 1024.0);
    ImGui::Text("GPU VRAM Used: %.0f MB / Free: %.0f MB", used, free);

    ImGui::Text("GPU Renderer:\n%s", m_sysMonitor.GetGPURenderer().c_str());
    ImGui::Text("OpenGL Version:\n%s", m_sysMonitor.GetGPUVersion().c_str());
    ImGui::Text(" ");

    ImGui::Text("Application average %.3f ms\nFrame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::EndChild();
}
