#include "../../include/debug/imgui_docking.h"

#include "../../include/misc/color_manager.h"

#include <string>
#include <format>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "themes/imgui_spectrum.h"

#include "extensions/toggle/imgui_toggle.h"
#include "extensions/toggle/imgui_toggle_palette.h"

#include "../../include/transform.h"

#include "../../include/lights/light.h"
#include "../../include/lights/point_light.h"
#include "../../include/lights/spot_light.h"
#include "../../include/lights/directional_light.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



const ImVec4 gray(0.882f, 0.882f, 0.882f, 1.0f);
const ImVec4 white(0.502f, 0.502f, 0.502f, 1.0f);
const ImVec4 dark(0.0f, 0.0f, 0.0f, 0.2f);
const ImVec4 light(1.0f, 1.0f, 1.0f, 0.2f);




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
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.38f, NULL, &dock_main_id);

		ImGui::DockBuilderDockWindow("Scene", dock_id_left);
        ImGui::DockBuilderDockWindow("Settings", dock_id_left);
		ImGui::DockBuilderDockWindow("About", dock_id_left);

		ImGui::DockBuilderDockWindow("Properties", dock_id_right);

		ImGui::DockBuilderFinish(dockspace_id);
	}

	ImGui::DockSpace(dockspace_id, ImGui::GetContentRegionAvail(), dockspace_flags);

	ImGui::End();

	ImGui::Begin("Scene", nullptr, 0);
    renderHierarchyWidget();
	ImGui::End();

    ImGui::Begin("Settings", nullptr, 0);
    renderTabSettings();
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
        displayEntityHierarchy(m_rootEntity);
        ImGui::EndChild();
    }
}

void engine::ImGuiDocking::renderPropertiesWidget()
{
    if (m_rootEntity)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f); // Set rounding to 5 pixels
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f)); // 10 pixels padding on x and y
        ImGui::BeginChild("EntityPropertyRegion", ImVec2(0, 0), true, ImGuiWindowFlags_None);
        displayEntityDetails(m_selectedEntity);
        ImGui::EndChild();
        ImGui::PopStyleVar(2); // Restore default
    }
}

void engine::ImGuiDocking::renderTabSettings()
{
    ImGui::BeginChild("SettingsRegion", ImVec2(0, 0), true);

    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    ImGuiTogglePalette material_palette_on;
    material_palette_on.Frame = dark;
    material_palette_on.Knob = white;
    material_palette_on.KnobHover = white;
    material_palette_on.FrameBorder = light;

    ImGuiTogglePalette material_palette_off;
    material_palette_off.Frame = dark;
    material_palette_off.Knob = gray;
    material_palette_off.KnobHover = white;
    material_palette_off.FrameBorder = light;

    ImGuiToggleConfig toggle_config;
    toggle_config.Flags |= ImGuiToggleFlags_Bordered | ImGuiToggleFlags_Animated;
    toggle_config.Size = ImVec2(30.0f, 18.0f);
    toggle_config.On.Palette = &material_palette_on;
    toggle_config.Off.Palette = &material_palette_off;

    ImGui::Spacing();


    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    if (ImGui::Toggle("Wireframe", &settings_wireframe, toggle_config))
    {
        if (m_onRenderModeSettingChanged)
            m_onRenderModeSettingChanged(settings_wireframe);
    }

    ImGui::PopStyleVar();

    ImGui::PopStyleColor();

    ImGui::EndChild();
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

void engine::ImGuiDocking::displayEntityHierarchy(const std::shared_ptr<Entity>& entity)
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
            displayEntityHierarchy(child);
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
        ImGui::Text("%s", entity->name.c_str());
        ImGui::PopStyleColor();

        // Type name directly below
        ImGui::Text("%s", entity->getTypeNameEx().c_str());

        ImGui::EndGroup();


        

        if (entity->getType() == engine::EntityType::light)
        {
            drawTransformEditor(entity->transform, true, false, false);
            drawLightEntityDetails(entity);
        }
        else if (entity->getType() == engine::EntityType::camera)
        {
            drawTransformEditor(entity->transform, true, true, true);
            drawCameraEntityDetails(entity);
        }
        else
        {
            drawTransformEditor(entity->transform, true, true, true);
        }
    }
}

// Draw the TRS editor
void engine::ImGuiDocking::drawTransformEditor(engine::Transform& transform, bool position, bool rotation, bool scale)
{
    ImGui::SeparatorText("Transform");

    static auto green = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.25f, 0.25f, 0.3f));
    static auto blue = ImGui::ColorConvertFloat4ToU32(ImVec4(0.25f, 0.25f, 1.0f, 0.3f));
    static auto red = ImGui::ColorConvertFloat4ToU32(ImVec4(0.25f, 1.0f, 0.25f, 0.3f));
    static auto white = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    
    if (ImGui::BeginTable("MyTable", 7, ImGuiTableFlags_SizingStretchSame))
    {
        ImGui::TableSetupColumn("Labels", ImGuiTableColumnFlags_WidthFixed, itemLabelWidth);
        ImGui::TableSetupColumn("vx", ImGuiTableColumnFlags_WidthFixed, 75.0f);
        ImGui::TableSetupColumn("vy", ImGuiTableColumnFlags_WidthFixed, 75.0f);
        ImGui::TableSetupColumn("vz", ImGuiTableColumnFlags_WidthFixed, 75.0f);

        if (position)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Position");

            
            ImGui::TableSetColumnIndex(1);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.25f, 0.25f, 0.3f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(1.0f, 0.25f, 0.25f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1.0f, 0.25f, 0.25f, 0.7f));
            ImGui::DragFloat("##posX", &transform.getLocalPosition().x, 0.1f);
            ImGui::PopStyleColor(3);

            
            ImGui::TableSetColumnIndex(2);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.25f, 1.0f, 0.25f, 0.3f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.25f, 1.0f, 0.25f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 1.0f, 0.25f, 0.7f));
            ImGui::DragFloat("##posY", &transform.getLocalPosition().y, 0.1f);
            ImGui::PopStyleColor(3);

            
            ImGui::TableSetColumnIndex(3);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.25f, 0.25f, 1.0f, 0.3f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.25f, 0.25f, 1.0f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 0.25f, 1.0f, 0.7f));
            ImGui::DragFloat("##posZ", &transform.getLocalPosition().z, 0.1f);
            ImGui::PopStyleColor(3);
        }


        if (rotation)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Rotation");

            
            ImGui::TableSetColumnIndex(1);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.25f, 0.25f, 0.3f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(1.0f, 0.25f, 0.25f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1.0f, 0.25f, 0.25f, 0.7f));
            ImGui::DragFloat("##rotX", &transform.getLocalRotation().x, 0.5f);
            ImGui::PopStyleColor(3);

            
            ImGui::TableSetColumnIndex(2);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.25f, 1.0f, 0.25f, 0.3f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.25f, 1.0f, 0.25f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 1.0f, 0.25f, 0.7f));
            ImGui::DragFloat("##rotY", &transform.getLocalRotation().y, 0.5f);
            ImGui::PopStyleColor(3);

            
            ImGui::TableSetColumnIndex(3);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.25f, 0.25f, 1.0f, 0.3f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.25f, 0.25f, 1.0f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 0.25f, 1.0f, 0.7f));
            ImGui::DragFloat("##rotZ", &transform.getLocalRotation().z, 0.5f);
            ImGui::PopStyleColor(3);
        }


        if (scale)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Scale");

            ImGui::TableSetColumnIndex(1);
            drawCustomDragFloat("X", "##scaX", ImGui::GetCursorScreenPos(), SIZE, ROUNDING, 50.0f, green, white, transform.getLocalScale().x, 0.05f);
            
            ImGui::TableSetColumnIndex(2);
            drawCustomDragFloat("Y", "##scaY", ImGui::GetCursorScreenPos(), SIZE, ROUNDING, 50.0f, red, white, transform.getLocalScale().y, 0.05f);
            
            ImGui::TableSetColumnIndex(3);
            drawCustomDragFloat("Z", "##scaZ", ImGui::GetCursorScreenPos(), SIZE, ROUNDING, 50.0f, blue, white, transform.getLocalScale().z, 0.05f);
        }

        ImGui::EndTable();
    }
}


void engine::ImGuiDocking::drawLightEntityDetails(const std::shared_ptr<Entity>& entity)
{
    if (entity && entity->light)
    {
        std::shared_ptr<PointLight> pointLight;
        std::shared_ptr<DirectionalLight> dirLight;
        std::shared_ptr<SpotLight> spotLight;
        
        if (pointLight = std::dynamic_pointer_cast<PointLight>(entity->light))
        {

        }
        else if (dirLight = std::dynamic_pointer_cast<DirectionalLight>(entity->light))
        {

        }
        else if (spotLight = std::dynamic_pointer_cast<SpotLight>(entity->light))
        {
        }


        if (dirLight != nullptr || spotLight != nullptr)
        {
            if (ImGui::BeginTable("MyTable", 7, ImGuiTableFlags_SizingStretchSame))
            {
                ImGui::TableSetupColumn("Labels", ImGuiTableColumnFlags_WidthFixed, itemLabelWidth);
                ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, 5.0f);
                ImGui::TableSetupColumn("vx", ImGuiTableColumnFlags_WidthFixed, 70.0f);
                ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed, 5.0f);
                ImGui::TableSetupColumn("vy", ImGuiTableColumnFlags_WidthFixed, 70.0f);
                ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthFixed, 5.0f);
                ImGui::TableSetupColumn("vz", ImGuiTableColumnFlags_WidthFixed, 70.0f);

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Target");

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("X");
                ImGui::TableSetColumnIndex(2);
                ImGui::DragFloat("##targetX", &spotLight->target.x, 0.1f);

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("Y");
                ImGui::TableSetColumnIndex(4);
                ImGui::DragFloat("##targetY", &spotLight->target.y, 0.1f);

                ImGui::TableSetColumnIndex(5);
                ImGui::Text("Z");
                ImGui::TableSetColumnIndex(6);
                ImGui::DragFloat("##targetZ", &spotLight->target.z, 0.1f);

                ImGui::EndTable();
            }
        }

        
        ImGui::SeparatorText("Detail");
        
        if (ImGui::BeginTable("MyTable", 2, ImGuiTableFlags_SizingStretchSame))
        {
            ImGui::TableSetupColumn("Labels", ImGuiTableColumnFlags_WidthFixed, itemLabelWidth);
            ImGui::TableSetupColumn("Controls", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Intensity");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(80);
            ImGui::DragFloat("##intensity", &entity->light->intensity, 1.0f, 0.0f, 1000.0f, "%.3f", ImGuiSliderFlags_None);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Ambient Color");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN); // Use all available width
            ImGui::ColorEdit3("##ambientColor", glm::value_ptr(entity->light->ambientColor), ImGuiColorEditFlags_NoLabel);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Diffuse Color");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN); // Use all available width
            ImGui::ColorEdit3("##diffuseColor", glm::value_ptr(entity->light->diffuseColor), ImGuiColorEditFlags_NoLabel);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Specular Color");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN); // Use all available width
            // Show color preview
            ImGui::ColorEdit3("##specularColor", glm::value_ptr(entity->light->specularColor), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::SameLine();

            // Show numeric inputs without labels
            ImGui::DragFloat3(
                "##specularColorInputs",
                glm::value_ptr(entity->light->specularColor),
                0.01f,
                0.0f,
                1.0f
            );

            ImGui::EndTable();
        }

        if (auto pointLight = std::dynamic_pointer_cast<PointLight>(entity->light))
        {

        }
        else if (auto dirLight = std::dynamic_pointer_cast<DirectionalLight>(entity->light))
        {
            
        }
        else if (auto spotLight = std::dynamic_pointer_cast<SpotLight>(entity->light))
        {
            

            if (ImGui::BeginTable("MyTable", 2, ImGuiTableFlags_SizingStretchSame))
            {
                ImGui::TableSetupColumn("Labels", ImGuiTableColumnFlags_WidthFixed, itemLabelWidth);
                ImGui::TableSetupColumn("Controls", ImGuiTableColumnFlags_WidthStretch);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Inner Cutoff");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(80);
                ImGui::DragFloat("##innerCutoff", &spotLight->cutoff, 1.0f, 0.0f, 1000.0f, "%.3f", ImGuiSliderFlags_None);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Outer Cutoff");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(80);
                ImGui::DragFloat("##outerCutoff", &spotLight->outerCutoff, 1.0f, 0.0f, 1000.0f, "%.3f", ImGuiSliderFlags_None);

                ImGui::EndTable();
            }
        }
    }
}

void engine::ImGuiDocking::drawCameraEntityDetails(const std::shared_ptr<Entity>& entity)
{
    if (entity && entity->camera)
    {
        if (ImGui::BeginTable("MyTable", 2, ImGuiTableFlags_SizingStretchSame))
        {
            ImGui::TableSetupColumn("Labels", ImGuiTableColumnFlags_WidthFixed, itemLabelWidth);
            ImGui::TableSetupColumn("Controls", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Zoom");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(80);
            ImGui::DragFloat("##zoom", &entity->camera->zoom, 1.0f, 0.0f, 1000.0f, "%.3f", ImGuiSliderFlags_None);

            ImGui::EndTable();
        }
    }
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


void engine::ImGuiDocking::drawCustomLabel(const char* text, const ImVec2& position, const ImVec2& size, float rounding, ImU32 backgroundColor, ImU32 foregroundColor)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 min = position;
    ImVec2 max = ImVec2(position.x + size.x, position.y + size.y);

    // Draw red rectangle with rounded left corners
    draw_list->AddRectFilled(min, max, backgroundColor, rounding, ImDrawFlags_RoundCornersLeft);

    // Draw text centered
    ImVec2 text_size = ImGui::CalcTextSize(text);
    ImVec2 text_pos = ImVec2(
        position.x + (size.x - text_size.x) * 0.5f,
        position.y + (size.y - text_size.y) * 0.5f
    );
    draw_list->AddText(text_pos, foregroundColor, text);
}


void engine::ImGuiDocking::drawCustomDragFloat(const char* text, const char* name, const ImVec2& position, const ImVec2& size, float rounding, float width, ImU32 backgroundColor, ImU32 foregroundColor, float& value, float step)
{
    drawCustomLabel(text, position, size, rounding, backgroundColor, foregroundColor);

    // Move cursor to the end of the label manually
    ImGui::SetCursorScreenPos(ImVec2(position.x + size.x, position.y));

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size2 = ImVec2(width, ImGui::GetFrameHeight()); // Width can be adjusted

    // Draw background with rounded right corners
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(pos, ImVec2(pos.x + size2.x, pos.y + size2.y),
        IM_COL32(50, 50, 50, 255), rounding,
        ImDrawFlags_RoundCornersRight);

    // Render the DragFloat widget
    ImGui::SetCursorScreenPos(pos); // Reset cursor to draw over the background
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0)); // Transparent background
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0, 0, 0, 0));
    ImGui::SetNextItemWidth(width);
    ImGui::DragFloat(name, &value, step);
    ImGui::PopStyleColor(3);
}
