#include "../../include/debug/imgui_docking.h"

#include "../../include/misc/color_manager.h"

#include <string>
#include <format>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "themes/imgui_spectrum.h"
#include "../../include/transform.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



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

void engine::ImGuiDocking::displayEntityInImGui(const std::shared_ptr<Entity>& entity)
{
    // Check if this entity is selected
    bool isSelected = (m_selectedEntity == entity);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DrawLinesFull;

    if (entity->children.empty())
        flags |= ImGuiTreeNodeFlags_Leaf;

    auto entityType = entity->getType();

    if (isSelected)
    {
        flags |= ImGuiTreeNodeFlags_Selected;
        ImGui::PushStyleColor(ImGuiCol_Text, getEntityColor(entityType));
    }

    ImGui::PushID(entity.get()); // Unique ID per entity

    GLuint iconTexture = getEntityTypeSmallIcon(entityType);

    // Start horizontal layout
    bool nodeOpen = false;
    ImGui::Image(iconTexture, ImVec2(16, 16));
    ImGui::SameLine();

    // Use TreeNodeEx with invisible label (##) to show triangle only
    nodeOpen = ImGui::TreeNodeEx("##tree", flags, "%s %s", entity->name.c_str(), (entity->transform.isDirty() ? "*" : ""));

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
        auto entityType = entity->getType();
        GLuint iconTexture = getEntityTypeMediumIcon(entityType);

        // Draw the icon
        ImGui::Image(iconTexture, ImVec2(48, 48));

        // Place next content on the same line as the image
        ImGui::SameLine();

        // Create a vertical group next to the image
        ImGui::BeginGroup();

        // Entity name with color
        ImGui::PushStyleColor(ImGuiCol_Text, getEntityColor(entityType));
        ImGui::Text("%s", m_selectedEntity->name.c_str());
        ImGui::PopStyleColor();

        // Type name directly below
        ImGui::Text("%s", m_selectedEntity->getTypeName().c_str());

        ImGui::EndGroup();

        ImGui::Separator();

        //static float posx;
        //ImGui::InputFloat("aaa", &posx, 0.1f, 0.5f, "%.3f");




        //ImGui::Text("Position = (%.2f, %.2f, %.2f)",
        //    m_selectedEntity->transform.getLocalPosition().x,
        //    m_selectedEntity->transform.getLocalPosition().y,
        //    m_selectedEntity->transform.getLocalPosition().z);

        //ImGui::Text("Rotation = (%.2f, %.2f, %.2f)",
        //    m_selectedEntity->transform.getLocalRotation().x,
        //    m_selectedEntity->transform.getLocalRotation().y,
        //    m_selectedEntity->transform.getLocalRotation().z);

        //ImGui::Text("Scale = (%.2f, %.2f, %.2f)",
        //    m_selectedEntity->transform.getLocalScale().x,
        //    m_selectedEntity->transform.getLocalScale().y,
        //    m_selectedEntity->transform.getLocalScale().z);


        drawTransformEditor(m_selectedEntity->transform);
    }
}

// Draw the TRS editor
void engine::ImGuiDocking::drawTransformEditor(engine::Transform& transform)
{
    // ---------------- Position ----------------
    ImGui::Text("Position");

    

    // X
    ImGui::SetNextItemWidth(itemWidth);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.25f, 0.25f, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(1.0f, 0.25f, 0.25f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1.0f, 0.25f, 0.25f, 0.7f));
    if (ImGui::DragFloat("X##pos", &transform.getLocalPosition().x, 0.1f))
    {
        transform.setDirty();
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();

    // Y
    ImGui::SetNextItemWidth(itemWidth);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.25f, 1.0f, 0.25f, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.25f, 1.0f, 0.25f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 1.0f, 0.25f, 0.7f));
    if (ImGui::DragFloat("Y##pos", &transform.getLocalPosition().y, 0.1f))
    {
        transform.setDirty();
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();

    // Z
    ImGui::SetNextItemWidth(itemWidth);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.25f, 0.25f, 1.0f, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.25f, 0.25f, 1.0f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 0.25f, 1.0f, 0.7f));
    if (ImGui::DragFloat("Z##pos", &transform.getLocalPosition().z, 0.1f))
    {
        transform.setDirty();
    }
    ImGui::PopStyleColor(3);

    



    // ---------------- Rotation ----------------
    ImGui::Text("Rotation");

    // X (Red)
    ImGui::SetNextItemWidth(itemWidth);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.25f, 0.25f, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(1.0f, 0.25f, 0.25f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1.0f, 0.25f, 0.25f, 0.7f));
    ImGui::DragFloat("X##rot", &transform.getLocalRotation().x, 0.5f);
    ImGui::PopStyleColor(3);

    ImGui::SameLine();

    // Y (Green)
    ImGui::SetNextItemWidth(itemWidth);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.25f, 1.0f, 0.25f, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.25f, 1.0f, 0.25f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 1.0f, 0.25f, 0.7f));
    ImGui::DragFloat("Y##rot", &transform.getLocalRotation().y, 0.5f);
    ImGui::PopStyleColor(3);

    ImGui::SameLine();

    // Z (Blue)
    ImGui::SetNextItemWidth(itemWidth);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.25f, 0.25f, 1.0f, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.25f, 0.25f, 1.0f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 0.25f, 1.0f, 0.7f));
    ImGui::DragFloat("Z##rot", &transform.getLocalRotation().z, 0.5f);
    ImGui::PopStyleColor(3);


    
    // ---------------- Scale ----------------
    ImGui::Text("Scale");


    // X (Red)
    ImGui::SetNextItemWidth(itemWidth);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.25f, 0.25f, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(1.0f, 0.25f, 0.25f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1.0f, 0.25f, 0.25f, 0.7f));
    ImGui::DragFloat("X##sca", &transform.getLocalScale().x, 0.05f);
    ImGui::PopStyleColor(3);

    ImGui::SameLine();

    // Y (Green)
    ImGui::SetNextItemWidth(itemWidth);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.25f, 1.0f, 0.25f, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.25f, 1.0f, 0.25f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 1.0f, 0.25f, 0.7f));
    ImGui::DragFloat("Y##sca", &transform.getLocalScale().y, 0.05f);
    ImGui::PopStyleColor(3);

    ImGui::SameLine();

    // Z (Blue)
    ImGui::SetNextItemWidth(itemWidth);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.25f, 0.25f, 1.0f, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.25f, 0.25f, 1.0f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 0.25f, 1.0f, 0.7f));
    ImGui::DragFloat("Z##sca", &transform.getLocalScale().z, 0.05f);
    ImGui::PopStyleColor(3);
}



GLuint engine::ImGuiDocking::getEntityTypeSmallIcon(const engine::EntityType entityType)
{
    auto it = m_iconSmallTextureCache.find(entityType);
    if (it != m_iconSmallTextureCache.end())
    {
        return it->second;
    }
    else {
        auto iconName = std::format("icon_{}_16x16.png", static_cast<int>(entityType));
        GLuint iconTexture = Texture::loadGLTextureFromFile(iconName.c_str(), "icons");

        m_iconSmallTextureCache.insert(std::make_pair(entityType, iconTexture));

        return iconTexture;
    }
}

GLuint engine::ImGuiDocking::getEntityTypeMediumIcon(const engine::EntityType entityType)
{
    auto it = m_iconMediumTextureCache.find(entityType);
    if (it != m_iconMediumTextureCache.end())
    {
        return it->second;
    }
    else {
        auto iconName = std::format("icon_{}_48x48.png", static_cast<int>(entityType));
        GLuint iconTexture = Texture::loadGLTextureFromFile(iconName.c_str(), "icons");

        m_iconMediumTextureCache.insert(std::make_pair(entityType, iconTexture));

        return iconTexture;
    }
}

ImVec4 engine::ImGuiDocking::getEntityColor(const engine::EntityType entityType)
{
    if (entityType == engine::EntityType::model)
    {
        auto purple = engine::ColorManager::hexToNormalizedRGB("#d478ff");
        return ImVec4(purple.r, purple.g, purple.b, purple.a);
    }
    else if (entityType == engine::EntityType::primitive)
    {
        auto green = engine::ColorManager::hexToNormalizedRGB("#abff78");
        return ImVec4(green.r, green.g, green.b, green.a);
    }
    else if (entityType == engine::EntityType::light)
    {
        auto yellow = engine::ColorManager::hexToNormalizedRGB("#ffd83b");
        return ImVec4(yellow.r, yellow.g, yellow.b, yellow.a);
    }
    else if (entityType == engine::EntityType::camera)
    {
        auto blue = engine::ColorManager::hexToNormalizedRGB("#0f9cff");
        return ImVec4(blue.r, blue.g, blue.b, blue.a);
    }

    return ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
}
