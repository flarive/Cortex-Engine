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


//#include "extensions/imoguizmo.hpp"
//#include <glm/gtc/type_ptr.hpp> // for glm::value_ptr

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






    //aaaaaaaaaaaaaaaaaaaa
    // it is recommended to use a separate projection matrix since the values that work best
    // can be very different from what works well with normal renderings
    // e.g., with glm -> glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f);

    // optional: configure color, axis length and more
    //ImOGuizmo::config.axisLengthScale = 1.0f;

    //// specify position and size of gizmo (and its window when using ImOGuizmo::BeginFrame())
    //ImOGuizmo::SetRect(0.0f /* x */, 0.0f /* y */, 120.0f /* square size */);
    //ImOGuizmo::BeginFrame(); // to use you own window remove this call 
    //// and wrap everything in between ImGui::Begin() and ImGui::End() instead



    //glm::mat4 projMat = glm::perspective(
    //    glm::radians(cam->zoom),
    //    static_cast<float>(1280) / static_cast<float>(720),
    //    0.1f,
    //    100.0f
    //);
    //const float* projPtr = glm::value_ptr(projMat);

    //glm::mat4 viewMatrix = cam->GetViewMatrix();
    //float* viewPtr = glm::value_ptr(viewMatrix);

    //float pivotDistance = 0.0f;
    //// optional: set distance to pivot (-> activates interaction)
    //if (ImOGuizmo::DrawGizmo(viewPtr, projPtr, pivotDistance /* optional: default = 0.0f */))
    //{
    //    // in case of user interaction viewMatrix gets updated
    //}
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

    ImGui::PushID(entity.get()); // unique ID per entity

    GLuint iconTexture = getEntityTypeSmallIcon(entityType);

    // Row start: small icon
    ImGui::Image((ImTextureID)(intptr_t)iconTexture, ImVec2(16, 16));
    ImGui::SameLine();

    // Tree node
    //ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    bool nodeOpen = ImGui::TreeNodeEx("##tree", flags, "%s", entity->name.c_str());
    //ImGui::PopStyleVar();

    // Handle selection
    if (ImGui::IsItemClicked())
    {
        m_selectedEntity = entity;

        if (m_onSelectionChanged)
            m_onSelectionChanged(m_selectedEntity);
    }

    // Image button at the end of the line
    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 28.0f); // align to right side
    GLuint buttonIcon = entity->visible ? getEntityActionIcon("hide") : getEntityActionIcon("show");

    


    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 0.f));
    
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(-2.f, 0.f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.1f, 0.1f));

    if (ImGui::ImageButton("##visible", (ImTextureID)(intptr_t)buttonIcon, ImVec2(16, 16)))
    {
        entity->visible = !entity->visible;
    }

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(4);



    if (isSelected)
        ImGui::PopStyleColor();

    // Draw children
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
        ImGui::Text("%s", m_selectedEntity->getTypeNameEx().c_str());

        ImGui::EndGroup();

        ImGui::Separator();

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
    ImGui::DragFloat("X##pos", &transform.getLocalPosition().x, 0.1f);
    ImGui::PopStyleColor(3);

    ImGui::SameLine();

    // Y
    ImGui::SetNextItemWidth(itemWidth);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.25f, 1.0f, 0.25f, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.25f, 1.0f, 0.25f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 1.0f, 0.25f, 0.7f));
    ImGui::DragFloat("Y##pos", &transform.getLocalPosition().y, 0.1f);
    ImGui::PopStyleColor(3);

    ImGui::SameLine();

    // Z
    ImGui::SetNextItemWidth(itemWidth);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.25f, 0.25f, 1.0f, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.25f, 0.25f, 1.0f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 0.25f, 1.0f, 0.7f));
    ImGui::DragFloat("Z##pos", &transform.getLocalPosition().z, 0.1f);
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

GLuint engine::ImGuiDocking::getEntityActionIcon(const std::string& key)
{
    auto it = m_iconActionTextureCache.find(key);
    if (it != m_iconActionTextureCache.end())
    {
        return it->second;
    }
    else {
        auto iconName = std::format("icon_{}_16x16.png", key);
        GLuint iconTexture = Texture::loadGLTextureFromFile(iconName.c_str(), "icons");

        m_iconActionTextureCache.insert(std::make_pair(key, iconTexture));

        return iconTexture;
    }
}