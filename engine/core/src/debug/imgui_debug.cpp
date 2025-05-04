#include "../../include/debug/imgui_debug.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "themes/imgui_spectrum.h"


void engine::ImGuiDebug::setScene(std::shared_ptr<Entity> rootEntity)
{
    m_rootEntity = rootEntity;

    m_selectedEntity = rootEntity;
}

void engine::ImGuiDebug::renderUIWindow(bool show)
{
    ImGui::SetNextWindowSize(ImVec2(480, 400), ImGuiCond_Once);

    ImGui::Begin("Cortex engine debugger", &show);

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("Scene"))
        {
            renderTabScene();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("About"))
        {
            renderTabAbout();
            ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
    }

    ImGui::End();
}

void engine::ImGuiDebug::renderTabScene()
{
    if (m_rootEntity)
    {
        ImGui::BeginChild("EntityTreeRegion", ImVec2(0, 200), true); // 300 pixels height
        displayEntityInImGui(m_rootEntity); // Your tree rendering function
        ImGui::EndChild();

        ImGui::BeginChild("EntityTreeDetailRegion", ImVec2(0, 150), true); // 300 pixels height
        displayEntityDetails(m_selectedEntity); // Your tree rendering function
        ImGui::EndChild();
    }
}

void engine::ImGuiDebug::renderTabAbout()
{
    ImGui::Text(" ");
    ImGui::Text("GPU Vendor: %s", m_sysMonitor.GetGPUVendor().c_str());
    ImGui::Text("GPU Renderer: %s", m_sysMonitor.GetGPURenderer().c_str());
    ImGui::Text("OpenGL Version: %s", m_sysMonitor.GetGPUVersion().c_str());
    ImGui::Text(" ");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

void engine::ImGuiDebug::displayEntityInImGui(const std::shared_ptr<Entity>& entity)
{
    // Check if this entity is selected
    bool isSelected = (m_selectedEntity == entity);


    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;

    if (m_selectedEntity == entity)
    {
        flags |= ImGuiTreeNodeFlags_Selected;

        // Change text color when selected
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f)); // Yellow text
    }

    // Render the node
    bool nodeOpen = ImGui::TreeNodeEx(entity->name.c_str(), flags);

    // Check for click to select
    if (ImGui::IsItemClicked())
    {
        m_selectedEntity = entity;
    }

    // Pop color if it was pushed
    if (isSelected)
    {
        ImGui::PopStyleColor();
    }

    if (nodeOpen)
    {
        for (const auto& child : entity->children)
        {
            displayEntityInImGui(child);
        }
        ImGui::TreePop();
    }
}

void engine::ImGuiDebug::displayEntityDetails(const std::shared_ptr<Entity>& entity)
{
    if (entity)
    {
        ImGui::Text("%s",
            m_selectedEntity->name.c_str());

        ImGui::Text("Position = (%.2f, %.2f, %.2f)",
            m_selectedEntity->transform.getLocalPosition().x,
            m_selectedEntity->transform.getLocalPosition().y,
            m_selectedEntity->transform.getLocalPosition().z);

        ImGui::Text("Rotation = (%.2f, %.2f, %.2f)",
            m_selectedEntity->transform.getLocalRotation().x,
            m_selectedEntity->transform.getLocalRotation().y,
            m_selectedEntity->transform.getLocalRotation().z);

        ImGui::Text("Scale = (%.2f, %.2f, %.2f)",
            m_selectedEntity->transform.getLocalScale().x,
            m_selectedEntity->transform.getLocalScale().y,
            m_selectedEntity->transform.getLocalScale().z);
    }
}