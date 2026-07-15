#include "../../include/editor/about_window.h"


void engine::AboutWindow::init()
{
    m_vramManager.init();

    // test PDH counters
    /*m_pdhCounters.listAll();
    m_pdhCounters.test();*/

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



    

    // --- SAMPLE EVERY 100 ms ---
    //static double lastSample = 0.0;
    //double now = glfwGetTime();

    //if (now - lastSample >= 0.1)   // 100 ms
    //{
    //    m_sysMonitor.update();     // updates internal PDH + RAM

    //    cachedCPU = m_sysMonitor.getCPUPDH();
    //    cachedCPUProcess = m_sysMonitor.getCPUProcess();
    //    cachedRAMUsed = m_sysMonitor.getRAMUsed();
    //    cachedRAMTotal = m_sysMonitor.getRAMTotal();
    //    cachedProcessRAM = m_sysMonitor.getProcessRAM();

    //    lastSample = now;
    //}

    static double accumulator = 0.0;
    static double lastTime = glfwGetTime();

    double now = glfwGetTime();
    double delta = now - lastTime;
    lastTime = now;

    accumulator += delta;

    if (accumulator >= 1.0)   // sample every 1 second
    {
        m_sysMonitor.update();

        cachedCPU = m_sysMonitor.getCPUPDH();
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



    // ImGui overlay
    ImGui::Text("CPU: %.0f %%", cachedCPU);
    ImGui::Text("RAM: %.2f / %.2f GB",
        cachedRAMUsed / (1024.0 * 1024 * 1024),
        cachedRAMTotal / (1024.0 * 1024 * 1024));


    ImGui::Text("App RAM: %.2f MB", cachedProcessRAM / (1024.0 * 1024.0));


    ImGui::Text("App CPU: %.1f %%", cachedCPUProcess);




    ImGui::EndChild();
}