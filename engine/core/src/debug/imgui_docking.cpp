#include "../../include/debug/imgui_docking.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <string>
#include <format>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "themes/imgui_spectrum.h"


void engine::ImGuiDocking::setScene(std::shared_ptr<Entity> rootEntity)
{
    m_rootEntity = rootEntity;

    m_selectedEntity = rootEntity;
}

/// <summary>
/// https://github.com/ocornut/imgui/issues/2109#issuecomment-430096134
/// </summary>
/// <param name="show"></param>
void engine::ImGuiDocking::renderUIWindow(bool show)
{
	bool open = true;

	static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    dockspace_flags |= ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);



	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", &open, window_flags);
	ImGui::PopStyleVar();

	ImGui::PopStyleVar(2);

	

	ImGuiID dockspace_id = ImGui::GetID("MyDockspace");

	if (ImGui::DockBuilderGetNode(dockspace_id) == NULL)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
		ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
		ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

        // This variable will track the document node, however we are not using it here as we aren't docking anything into it
		ImGuiID dock_main_id = dockspace_id;

		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.22f, NULL, &dock_main_id);
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.30f, NULL, &dock_main_id);

		ImGui::DockBuilderDockWindow("Scene", dock_id_left);
		ImGui::DockBuilderDockWindow("About", dock_id_left);

		ImGui::DockBuilderDockWindow("Properties", dock_id_right);

		ImGui::DockBuilderFinish(dockspace_id);
	}

	ImGui::DockSpace(dockspace_id, ImGui::GetContentRegionAvail(), dockspace_flags);

	ImGui::End();

	ImGui::Begin("Scene", nullptr, 0);
    renderHierarchyWidget();
	ImGui::End();

	ImGui::Begin("About", nullptr, 0);
    renderTabAbout();
	ImGui::End();

	ImGui::Begin("Properties", nullptr, 0);
    renderPropertiesWidget();
	ImGui::End();
}


void engine::ImGuiDocking::renderHierarchyWidget()
{
    if (m_rootEntity)
    {
        ImGui::BeginChild("EntityTreeRegion", ImVec2(0, 0), true);
        displayEntityInImGui(m_rootEntity);
        ImGui::EndChild();
    }
}

void engine::ImGuiDocking::renderPropertiesWidget()
{
    if (m_rootEntity)
    {
        ImGui::BeginChild("EntityPropertyRegion", ImVec2(0, 300), true);
        displayEntityDetails(m_selectedEntity);
        ImGui::EndChild();
    }
}

void engine::ImGuiDocking::renderTabAbout()
{
    ImGui::BeginChild("AboutRegion", ImVec2(0, 0), true);
    ImGui::Text("GPU Vendor:\n%s", m_sysMonitor.GetGPUVendor().c_str());
    ImGui::Text("GPU Renderer:\n%s", m_sysMonitor.GetGPURenderer().c_str());
    ImGui::Text("OpenGL Version:\n%s", m_sysMonitor.GetGPUVersion().c_str());
    ImGui::Text(" ");
    ImGui::Text("Application average %.3f ms\nFrame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::EndChild();
}

//void engine::ImGuiDocking::displayEntityInImGui(const std::shared_ptr<Entity>& entity)
//{
//    // Check if this entity is selected
//    bool isSelected = (m_selectedEntity == entity);
//
//
//    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DrawLinesFull;
//
//    if (m_selectedEntity == entity)
//    {
//        flags |= ImGuiTreeNodeFlags_Selected;
//
//        // Change text color when selected
//        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f)); // Yellow text
//    }
//
//    if (entity->children.size() == 0)
//        flags |= ImGuiTreeNodeFlags_Leaf;
//
//    // Render the node
//    bool nodeOpen = ImGui::TreeNodeEx(entity->name.c_str(), flags);
//
//    // Check for click to select
//    if (ImGui::IsItemClicked())
//        m_selectedEntity = entity;
//
//    // Pop color if it was pushed
//    if (isSelected)
//        ImGui::PopStyleColor();
//
//    if (nodeOpen)
//    {
//        for (const auto& child : entity->children)
//        {
//            displayEntityInImGui(child);
//        }
//        ImGui::TreePop();
//    }
//}

void engine::ImGuiDocking::displayEntityInImGui(const std::shared_ptr<Entity>& entity)
{
    // Check if this entity is selected
    bool isSelected = (m_selectedEntity == entity);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DrawLinesFull;

    if (entity->children.empty())
        flags |= ImGuiTreeNodeFlags_Leaf;

    if (isSelected)
    {
        flags |= ImGuiTreeNodeFlags_Selected;
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f)); // Yellow text
    }

    ImGui::PushID(entity.get()); // Unique ID per entity

    GLuint iconTexture = getEntityTypeIcon(entity->getType());

    // Start horizontal layout
    bool nodeOpen = false;
    ImGui::Image(iconTexture, ImVec2(16, 16));
    ImGui::SameLine();

    // Use TreeNodeEx with invisible label (##) to show triangle only
    nodeOpen = ImGui::TreeNodeEx("##tree", flags, "%s", entity->name.c_str());

    // Handle selection
    if (ImGui::IsItemClicked())
        m_selectedEntity = entity;

    if (isSelected)
        ImGui::PopStyleColor();

    if (nodeOpen)
    {
        for (const auto& child : entity->children)
            displayEntityInImGui(child);

        ImGui::TreePop();
    }

    ImGui::PopID();
}


void engine::ImGuiDocking::displayEntityDetails(const std::shared_ptr<Entity>& entity)
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

GLuint engine::ImGuiDocking::getEntityTypeIcon(const engine::EntityType entityType)
{
    auto it = m_map.find(entityType);
    if (it != m_map.end())
    {
        return it->second;
    }
    else {
        auto iconName = std::format("icon_{}_20x20.png", static_cast<int>(entityType));
        GLuint iconTexture = Texture::loadGLTextureFromFile(iconName.c_str(), "icons");

        m_map.insert(std::make_pair(entityType, iconTexture));
    }
}
