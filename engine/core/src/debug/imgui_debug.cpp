#include "../../include/debug/imgui_debug.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "themes/imgui_spectrum.h"

void engine::ImGuiDebug::renderUIWindow(bool show)
{
    ImGui::SetNextWindowSize(ImVec2(480, 260), ImGuiCond_Always);

    ImGui::Begin("Cortex engine debugger", &show);



    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("Avocado"))
        {
            ImGui::Text(" ");
            ImGui::Text("GPU Vendor: %s", sysMonitor.GetGPUVendor().c_str());
            ImGui::Text("GPU Renderer: %s", sysMonitor.GetGPURenderer().c_str());
            ImGui::Text("OpenGL Version: %s", sysMonitor.GetGPUVersion().c_str());
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Broccoli"))
        {
            //A tree with 30000 leaf to draw
            int nLeafNum = 30000;
            if (ImGui::TreeNodeEx("Large Tree"))
            {
                for (int i = 0; i < nLeafNum; i++)
                {
                    //the algorithm for rendering 
                    //when index  is a multiple of ten 
                    //draw red as a key object
                    auto strLeafName = std::to_string(i);
                    bool bIsKey = i % 10 == 0;
                    ImGui::PushID(0); ImGui::PushStyleColor(ImGuiCol_Text,
                        bIsKey ? ImVec4(1.0f, 0.0f, 0.0f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 0.8f));

                    if (ImGui::TreeNodeEx(strLeafName.c_str(), ImGuiTreeNodeFlags_Leaf))
                    {
                        ImGui::TreePop();
                    }
                    ImGui::PopStyleColor(1); ImGui::PopID();
                }
                ImGui::TreePop();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Cucumber"))
        {
            ImGui::Text("This is the Cucumber tab!\nblah blah blah blah blah");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::Separator(); 


    //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);
    ImGui::End();
}