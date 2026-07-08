#include "../../include/editor/dockspace.h"

void engine::DockSpaceElement::begin()
{
	bool open = true;

	ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground;   // IMPORTANT

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 5.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));

	ImGui::Begin("DockSpace", &open, window_flags);

	ImGui::PopStyleVar(3);
}

void engine::DockSpaceElement::draw()
{
	ImGuiID dockspace_id = ImGui::GetID("MyDockspace");

	if (!ImGui::DockBuilderGetNode(dockspace_id))
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::DockBuilderRemoveNode(dockspace_id);
		ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

		ImGuiID dock_main_id = dockspace_id;

		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.22f, NULL, &dock_main_id);
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.38f, NULL, &dock_main_id);
		ImGuiID dock_id_top = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.10f, NULL, &dock_main_id);

		ImGui::DockBuilderDockWindow("Scene", dock_id_left);
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
		ImGui::DockBuilderDockWindow("About", dock_id_left);

		ImGui::DockBuilderDockWindow("Properties", dock_id_right);
		ImGui::DockBuilderDockWindow("FloatingToolbar", dock_id_top);

		ImGui::DockBuilderFinish(dockspace_id);
	}

	ImGui::DockSpace(dockspace_id, ImGui::GetContentRegionAvail());
}

void engine::DockSpaceElement::end()
{
    ImGui::End();
}