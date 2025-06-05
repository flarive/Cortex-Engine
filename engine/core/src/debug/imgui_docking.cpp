#include "../../include/debug/imgui_docking.h"

#include "imgui.h"
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

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &open, window_flags);
	ImGui::PopStyleVar();

	ImGui::PopStyleVar(2);

	if (ImGui::DockBuilderGetNode(ImGui::GetID("MyDockspace")) == NULL)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
		ImGui::DockBuilderAddNode(dockspace_id, viewport->Size); // Add empty node

		ImGuiID dock_main_id = dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, NULL, &dock_main_id);
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, NULL, &dock_main_id);
		ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, NULL, &dock_main_id);

		ImGui::DockBuilderDockWindow("James_1", dock_id_left);
		ImGui::DockBuilderDockWindow("James_2", dock_main_id);
		ImGui::DockBuilderDockWindow("James_3", dock_id_right);
		ImGui::DockBuilderDockWindow("James_4", dock_id_bottom);
		ImGui::DockBuilderFinish(dockspace_id);
	}

	ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
	ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0);
	ImGui::PopStyleColor();
	ImGui::End();

	ImGui::Begin("James_1", &open, 0);
	ImGui::Text("Text 1");
	ImGui::End();

	ImGui::Begin("James_2", &open, 0);
	ImGui::Text("Text 2");
	ImGui::End();

	ImGui::Begin("James_3", &open, 0);
	ImGui::Text("Text 3");
	ImGui::End();

	ImGui::Begin("James_4", &open, 0);
	ImGui::Text("Text 4");
	ImGui::End();
}