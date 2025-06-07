#include "../../include/debug/imgui_docking.h"

#include "imgui.h"
#include "imgui_internal.h"

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
	bool open1 = true;
	bool open2 = false;
	bool open3 = false;

	static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton;


	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &open, window_flags);
	ImGui::PopStyleVar();

	ImGui::PopStyleVar(2);

	

	ImGuiID dockspace_id = ImGui::GetID("MyDockspace");

	if (ImGui::DockBuilderGetNode(dockspace_id) == NULL)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
		ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
		ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

		ImGuiID dock_main_id = dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.22f, NULL, &dock_main_id);
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.30f, NULL, &dock_main_id);

		ImGui::DockBuilderDockWindow("TabHierarchy", dock_id_left);
		//ImGui::DockBuilderDockWindow("TabSearch", dock_id_left);     // New tab

		ImGui::DockBuilderDockWindow("TabProperties", dock_id_right);
		ImGui::DockBuilderFinish(dockspace_id);
	}

	

	ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ImVec4(0.0f, 0.0f, 0.0f, 0.5f));
	ImGui::DockSpace(dockspace_id, ImGui::GetContentRegionAvail(), dockspace_flags);
	ImGui::PopStyleColor();

	ImGui::End();

	ImGui::Begin("TabHierarchy", &open, 0);
    renderTabScene();
	//ImGui::Text("Avail: %.1f x %.1f", ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
	ImGui::End();

	//ImGui::Begin("TabSearch", &open2, 0);
	//ImGui::Text("This is the search tab.");
	//ImGui::End();

	ImGui::Begin("TabProperties", &open, 0);
    renderTabAbout();
	ImGui::End();
}


void engine::ImGuiDocking::renderTabScene()
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

void engine::ImGuiDocking::renderTabAbout()
{
    ImGui::Text(" ");
    ImGui::Text("GPU Vendor: %s", m_sysMonitor.GetGPUVendor().c_str());
    ImGui::Text("GPU Renderer: %s", m_sysMonitor.GetGPURenderer().c_str());
    ImGui::Text("OpenGL Version: %s", m_sysMonitor.GetGPUVersion().c_str());
    ImGui::Text(" ");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

void engine::ImGuiDocking::displayEntityInImGui(const std::shared_ptr<Entity>& entity)
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