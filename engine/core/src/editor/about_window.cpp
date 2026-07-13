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



    m_sysMonitor.update();   // updates CPU + RAM



    ImGui::Text("GPU Renderer:\n%s", m_sysMonitor.GetGPURenderer().c_str());
    ImGui::Text("OpenGL Version:\n%s", m_sysMonitor.GetGPUVersion().c_str());
    ImGui::Text(" ");

    ImGui::Text("Application average %.3f ms\nFrame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    


    //static double last = 0;
    //double now = glfwGetTime();

    //if (now - last > 0.2) {   // sample every 200ms
    //    m_sysMonitor.update();
    //    last = now;
    //}

    

    double cpu = m_sysMonitor.getCPU();
    double cpuProcess = m_sysMonitor.getCPUProcess();


    uint64_t ramUsed = m_sysMonitor.getRAMUsed();
    uint64_t ramTotal = m_sysMonitor.getRAMTotal();
    uint64_t processRamUsed = m_sysMonitor.getProcessRAM();

    // ImGui overlay
    ImGui::Text("CPU: %.1f %%", cpu);
    ImGui::Text("RAM: %.2f / %.2f GB",
        ramUsed / (1024.0 * 1024 * 1024),
        ramTotal / (1024.0 * 1024 * 1024));


    ImGui::Text("App RAM: %.2f MB", processRamUsed / (1024.0 * 1024.0));


    ImGui::Text("App CPU: %.1f %%", cpuProcess);


    // every second or so
    double percent = m_cpuUsage.GetUsagePercent();
    ImGui::Text("CPU GetUsagePercent: %.1f%%", percent);




    ImGui::EndChild();
}

