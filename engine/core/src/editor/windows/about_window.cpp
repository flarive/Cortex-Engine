#include "../../../include/editor/windows/about_window.h"

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




    ImGui::Text("GPU Renderer:\n%s", m_sysMonitor.GetGPURenderer().c_str());
    ImGui::Text("OpenGL Version:\n%s", m_sysMonitor.GetGPUVersion().c_str());
    ImGui::Text(" ");

    ImGui::Text("Application average %.3f ms\nFrame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);




    ImGui::Text("CPU total : %.0f %%", cachedCPU);
    ImGui::Text("CPU app : %.0f %%", cachedCPUProcess);


    ImGui::Text("RAM total : %.2f / %.2f GB", cachedRAMUsed / (1024.0 * 1024 * 1024), cachedRAMTotal / (1024.0 * 1024 * 1024));
    ImGui::Text("RAM app : %.2f MB", cachedProcessRAM / (1024.0 * 1024.0));

    ImGui::EndChild();
}