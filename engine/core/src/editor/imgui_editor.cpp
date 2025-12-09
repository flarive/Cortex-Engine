#include "../../include/editor/imgui_editor.h"

#include "../../include/misc/colors.h"

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

#include "../../include/primitives/primitive.h"
#include "../../include/primitives/sphere.h"
#include "../../include/primitives/cube.h"
#include "../../include/primitives/plane.h"
#include "../../include/primitives/cylinder.h"
#include "../../include/primitives/cone.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui_internal.h>

#include <format>
#include <unordered_map>
#include <functional>

#if EDITOR_MODE

const ImVec4 white(0.882f, 0.882f, 0.882f, 1.0f);
const ImVec4 gray(0.502f, 0.502f, 0.502f, 1.0f);
const ImVec4 dark(0.0f, 0.0f, 0.0f, 0.2f);
const ImVec4 light(1.0f, 1.0f, 1.0f, 0.2f);


void engine::ImGuiEditor::setScene(std::shared_ptr<Entity> rootEntity)
{
    m_rootEntity = rootEntity;
    m_selectedEntity = rootEntity;
}

/// <summary>
/// https://github.com/ocornut/imgui/issues/2109#issuecomment-430096134
/// </summary>
/// <param name="show"></param>
void engine::ImGuiEditor::renderUIWindow(bool show)
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

void engine::ImGuiEditor::renderHierarchyWidget()
{
    if (m_rootEntity)
    {
        ImGui::BeginChild("EntityTreeRegion", ImVec2(0, 0), true);
        displayEntityHierarchy(m_rootEntity);
        ImGui::EndChild();
    }
}

void engine::ImGuiEditor::renderPropertiesWidget()
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

void engine::ImGuiEditor::renderTabSettings()
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
    material_palette_off.KnobHover = gray;
    material_palette_off.FrameBorder = light;

    ImGuiToggleConfig toggle_config;
    toggle_config.Flags |= ImGuiToggleFlags_Bordered | ImGuiToggleFlags_Animated;
    toggle_config.Size = ImVec2(30.0f, 18.0f);
    toggle_config.On.Palette = &material_palette_on;
    toggle_config.Off.Palette = &material_palette_off;

    ImGui::Spacing();


    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));


    static bool lastDrawWireframe = false;
    if (ImGui::Toggle("Wireframe", &sceneSetting_drawAsWireframe, toggle_config))
    {
        if (m_onSceneSettingChanged && lastDrawWireframe != sceneSetting_drawAsWireframe)
        {
            m_onSceneSettingChanged("draw_wireframe", sceneSetting_drawAsWireframe);
            lastDrawWireframe = sceneSetting_drawAsWireframe;
        }
    }

    static bool lastEnableGammaCorection = false;
    if (ImGui::Toggle("Gamma correction", &sceneSetting_enableGammaCorrection, toggle_config))
    {
        if (m_onSceneSettingChanged && lastEnableGammaCorection != sceneSetting_enableGammaCorrection)
        {
            m_onSceneSettingChanged("enable_gamma_correction", sceneSetting_enableGammaCorrection);
            lastEnableGammaCorection = sceneSetting_enableGammaCorrection;
        }
    }

    static bool lastEnableFaceCulling = true;
    if (ImGui::Toggle("Face culling", &sceneSetting_enableFaceCulling, toggle_config))
    {
        if (m_onSceneSettingChanged && lastEnableFaceCulling != sceneSetting_enableFaceCulling)
        {
            m_onSceneSettingChanged("enable_face_culling", sceneSetting_enableFaceCulling);
			lastEnableFaceCulling = sceneSetting_enableFaceCulling;
        }
    }

    static bool lastEnableCameraFrustrumCulling = true;
    if (ImGui::Toggle("Camera frustrum culling", &sceneSetting_enableCameraFrustrumCulling, toggle_config))
    {
        if (m_onSceneSettingChanged && lastEnableCameraFrustrumCulling != sceneSetting_enableCameraFrustrumCulling)
        {
            m_onSceneSettingChanged("enable_camera_frustrum_culling", sceneSetting_enableCameraFrustrumCulling);
            lastEnableCameraFrustrumCulling = sceneSetting_enableCameraFrustrumCulling;
        }
    }

    static bool lastDrawLightsVisualHelpers = false;
    if (ImGui::Toggle("Lights visual helpers", &sceneSetting_drawLightsVisualHelpers, toggle_config))
    {
        if (m_onSceneSettingChanged && lastDrawLightsVisualHelpers != sceneSetting_drawLightsVisualHelpers)
        {
            m_onSceneSettingChanged("draw_lights_visual_helpers", sceneSetting_drawLightsVisualHelpers);
            lastDrawLightsVisualHelpers = sceneSetting_drawLightsVisualHelpers;
        }
    }

    static bool lastDrawBoundingBoxesVisualHelpers = false;
    if (ImGui::Toggle("Bounding boxes visual helpers", &sceneSetting_drawBoundingBoxesVisualHelpers, toggle_config))
    {
        if (m_onSceneSettingChanged && lastDrawBoundingBoxesVisualHelpers != sceneSetting_drawBoundingBoxesVisualHelpers)
        {
            m_onSceneSettingChanged("draw_bounding_boxes_visual_helpers", sceneSetting_drawBoundingBoxesVisualHelpers);
            lastDrawBoundingBoxesVisualHelpers = sceneSetting_drawBoundingBoxesVisualHelpers;
        }
    }

    static bool lastDrawDebugNormalsVisualHelpers = false;
    if (ImGui::Toggle("Normals visual helpers", &sceneSetting_drawDebugNormalsVisualHelpers, toggle_config))
    {
        if (m_onSceneSettingChanged && lastDrawDebugNormalsVisualHelpers != sceneSetting_drawDebugNormalsVisualHelpers)
        {
            m_onSceneSettingChanged("draw_debug_normals_visual_helpers", sceneSetting_drawDebugNormalsVisualHelpers);
            lastDrawDebugNormalsVisualHelpers = sceneSetting_drawDebugNormalsVisualHelpers;
        }
    }


    static bool lastEnableShadows = true;
    if (ImGui::Toggle("Enable shadows", &sceneSetting_enableShadows, toggle_config))
    {
        if (m_onSceneSettingChanged && lastEnableShadows != sceneSetting_enableShadows)
        {
            m_onSceneSettingChanged("enable_shadows", sceneSetting_enableShadows);
            lastEnableShadows = sceneSetting_enableShadows;
        }
    }

    

    static int lastShadowCalculationMethod = static_cast<int>(ShadowCalculationMethod::PCFSoft);
    renderSliderIntWithLabel("Shadow maps method", "shadow_calculation_method", sceneSetting_shadowCalculationMethod, lastShadowCalculationMethod, 1, 3);

    static int lastShadowMapTextureSize = 2048;
    renderSliderIntWithLabel("Shadow maps texture size", "shadow_maps_texture_size", sceneSetting_shadowMapTextureSize, lastShadowMapTextureSize, 256, 4096);

    static float lastShadowIntensity = 1.5f;
    renderDragFloatWithLabel("Shadow maps intensity", "shadow_intensity", sceneSetting_shadowIntensity, lastShadowIntensity, 0.0f, 5.0f, 0.1f, "%.1f");

    static float lastShadowMapsBiasFactor = 0.001f;
    renderDragFloatWithLabel("Shadow maps bias", "shadow_maps_bias_factor", sceneSetting_shadowMapBiasFactor, lastShadowMapsBiasFactor, 0.0001f, 0.001f, 0.0001f, "%.3f");

    static float lastShadowMapsBlur = 1.0f;
    renderDragFloatWithLabel("Shadow maps blur", "shadow_maps_blur_factor", sceneSetting_shadowMapBlur, lastShadowMapsBlur, 0.0f, 50.0f, 0.1f, "%.1f");

    ImGui::PopStyleVar();

    ImGui::PopStyleColor();

    ImGui::EndChild();
}

void engine::ImGuiEditor::renderSliderIntWithLabel(const char* label, const char* key, int& value, int& lastValue, int min, int max)
{
    static bool isDraggingSlider = false;

    // Use DragInt with a step of 256 (or your desired step)
    ImGui::SliderInt(
        label,
        &value,
        min,  // Minimum value
        max   // Maximum value
    );
    isDraggingSlider = ImGui::IsItemActive();

    // Apply changes only on release
    if (!isDraggingSlider && ImGui::IsItemDeactivatedAfterEdit())
    {
        if (m_onSceneSettingChanged && lastValue != value)
        {
            m_onSceneSettingChanged(key, value);
            lastValue = value;
        }
    }
}

void engine::ImGuiEditor::renderSliderFloatWithLabel(const char* label, const char* key, float& value, float& lastValue, float min, float max, const char* format)
{
    static bool isDraggingSlider = false;

    // Use DragInt with a step of 256 (or your desired step)
    ImGui::SliderFloat(
        label,
        &value,
        min,  // Minimum value
        max,   // Maximum value
        format // Display format
    );
    isDraggingSlider = ImGui::IsItemActive();

    // Apply changes only on release
    if (!isDraggingSlider && ImGui::IsItemDeactivatedAfterEdit())
    {
        if (m_onSceneSettingChanged && lastValue != value)
        {
            m_onSceneSettingChanged(key, value);
            lastValue = value;
        }
    }
}

void engine::ImGuiEditor::renderDragFloatWithLabel(const char* label, const char* key, float& value, float& lastValue, float min, float max, float step, const char* format)
{
    static bool isDraggingSlider = false;

    // Use DragInt with a step of 256 (or your desired step)
    ImGui::DragFloat(
        label,
        &value,
        step, // Step size (1.0f means it increments by 1 per "tick", but you can use 256.0f for 256 steps)
        min,  // Minimum value
        max,   // Maximum value
        format // Display format
    );
    isDraggingSlider = ImGui::IsItemActive();

    // Apply changes only on release
    if (!isDraggingSlider && ImGui::IsItemDeactivatedAfterEdit())
    {
        if (m_onSceneSettingChanged && lastValue != value)
        {
            m_onSceneSettingChanged(key, value);
            lastValue = value;
        }
    }
}

void engine::ImGuiEditor::renderTabAbout()
{
    ImGui::BeginChild("AboutRegion", ImVec2(0, 0), true);
    ImGui::Text("GPU Vendor:\n%s", m_sysMonitor.GetGPUVendor().c_str());
    ImGui::Text("GPU Renderer:\n%s", m_sysMonitor.GetGPURenderer().c_str());
    ImGui::Text("OpenGL Version:\n%s", m_sysMonitor.GetGPUVersion().c_str());
    ImGui::Text(" ");
    ImGui::Text("Application average %.3f ms\nFrame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::EndChild();
}

void engine::ImGuiEditor::displayEntityHierarchy(const std::shared_ptr<Entity>& entity)
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
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(11.f, 0.f));
    bool nodeOpen = ImGui::TreeNodeEx("##tree", flags, "%s", entity->name.c_str());
    ImGui::PopStyleVar(1);

    // Handle selection
    if (ImGui::IsItemClicked())
    {
        m_selectedEntity = entity;

        if (m_onSelectionChanged)
            m_onSelectionChanged(m_selectedEntity);
    }

    // Image button at the end of the line
    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 28.0f); // align to right side
    GLuint buttonIcon = entity->enabled ? getEntityActionIcon("hide") : getEntityActionIcon("show");

    


    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 0.f));
    
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(-2.f, 0.f));

    if (ImGui::ImageButton("##visible", (ImTextureID)(intptr_t)buttonIcon, ImVec2(16, 16)))
    {
        entity->setEnabled(!entity->enabled);
    }

    ImGui::PopStyleVar(1);
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

void engine::ImGuiEditor::displayEntityDetails(const std::shared_ptr<Entity>& entity)
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

        renderComponents(entity);
    }
}


void engine::ImGuiEditor::renderComponents(const std::shared_ptr<Entity>& entity)
{
    std::shared_ptr<TransformComponent> transformComponent{};

    // looping over entity components
    for (auto& [typeID, component] : entity->components)
    {
        if (typeID == ComponentType::transform)
        {
            // transform component
            transformComponent = std::reinterpret_pointer_cast<TransformComponent>(component);
            renderTransformComponent(entity, true, true, true);
        }
        else if (typeID == ComponentType::camera)
        {
            // camera component
            auto cameraComponent = dynamic_pointer_cast<CameraComponent>(component);
            if (cameraComponent) renderCameraComponent(cameraComponent);
        }
        else if (typeID == ComponentType::light)
        {
            // camera component
            auto lightComponent = dynamic_pointer_cast<LightComponent>(component);
            if (lightComponent) renderLightComponent(lightComponent);
        }
        else if (typeID == ComponentType::model)
        {
            // model component
            auto modelComponent = dynamic_pointer_cast<ModelComponent>(component);
            if (modelComponent) renderModelComponent(modelComponent);
        }
        else if (typeID == ComponentType::primitive)
        {
            // primitive component
            auto primitiveComponent = dynamic_pointer_cast<PrimitiveComponent>(component);
            if (primitiveComponent) renderPrimitiveComponent(primitiveComponent, transformComponent);
        }
    }
}


void engine::ImGuiEditor::renderTransformComponent(const std::shared_ptr<Entity>& entity, bool displayPosition, bool displayRotation, bool displayScale)
{
    auto transformComponent = entity->getComponent<TransformComponent>();


    ImGui::SeparatorText(transformComponent->getName().c_str());

    if (!transformComponent)
        return;

    static auto green = IM_COL32(138, 219, 0, 255);
    static auto blue = IM_COL32(44, 143, 255, 255);
    static auto red = IM_COL32(255, 54, 83, 255);
    static auto white = IM_COL32(255, 255, 255, 255);

    glm::vec3 position{0,0,0};
    glm::vec3 rotation{0,0,0};
    glm::vec3 scale{1,1,1};

    std::shared_ptr<CameraComponent> cameraComponent{};
    std::shared_ptr<LightComponent> lightComponent{};
    std::shared_ptr<PrimitiveComponent> primitiveComponent{};
    std::shared_ptr<ModelComponent> modelComponent{};

    //auto t = entity->getType();
    if (cameraComponent = entity->getComponent<CameraComponent>())
    {
        position = cameraComponent->getCamera()->position;
    }
    else if (lightComponent = entity->getComponent<LightComponent>())
    {
        position = lightComponent->getLight()->position;
    }
    else if (primitiveComponent = entity->getComponent<PrimitiveComponent>())
    {
        position = primitiveComponent->getPrimitive()->position;
        scale = primitiveComponent->getPrimitive()->scale;
        rotation = primitiveComponent->getPrimitive()->rotation;
    }
    else if (modelComponent = entity->getComponent<ModelComponent>())
    {
        position = modelComponent->getModel()->position;
        scale = modelComponent->getModel()->scale;
        rotation = modelComponent->getModel()->rotation;
    }

    // Local variables for ImGui
    float posX = position.x;
    float posY = position.y;
    float posZ = position.z;

    float rotX = rotation.x;
    float rotY = rotation.y;
    float rotZ = rotation.z;

    float scaX = scale.x;
    float scaY = scale.y;
    float scaZ = scale.z;

   
    if (ImGui::BeginTable("MyTable", 4, ImGuiTableFlags_SizingStretchSame))
    {
        ImGui::TableSetupColumn("Labels", ImGuiTableColumnFlags_WidthFixed, itemLabelWidth);
        ImGui::TableSetupColumn("vx", ImGuiTableColumnFlags_WidthFixed, 75.0f);
        ImGui::TableSetupColumn("vy", ImGuiTableColumnFlags_WidthFixed, 75.0f);
        ImGui::TableSetupColumn("vz", ImGuiTableColumnFlags_WidthFixed, 75.0f);

        if (displayPosition)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Position");

            ImGui::TableSetColumnIndex(1);
            if (drawCustomDragFloat("X", "##posX", ImGui::GetCursorScreenPos(), SIZE, ROUNDING, 50.0f, green, white, &posX, 0.01f)) {
                position.x = posX;
            }

            ImGui::TableSetColumnIndex(2);
            if (drawCustomDragFloat("Y", "##posY", ImGui::GetCursorScreenPos(), SIZE, ROUNDING, 50.0f, red, white, &posY, 0.01f)) {
                position.y = posY;
            }

            ImGui::TableSetColumnIndex(3);
            if (drawCustomDragFloat("Z", "##posZ", ImGui::GetCursorScreenPos(), SIZE, ROUNDING, 50.0f, blue, white, &posZ, 0.01f)) {
                position.z = posZ;
            }
        }

        if (displayRotation)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Rotation");

            ImGui::TableSetColumnIndex(1);
            if (drawCustomDragFloat("X", "##rotX", ImGui::GetCursorScreenPos(), SIZE, ROUNDING, 50.0f, green, white, &rotX, 1.0f)) {
                rotation.x = rotX;
            }

            ImGui::TableSetColumnIndex(2);
            if (drawCustomDragFloat("Y", "##rotY", ImGui::GetCursorScreenPos(), SIZE, ROUNDING, 50.0f, red, white, &rotY, 1.0f)) {
                rotation.y = rotY;
            }

            ImGui::TableSetColumnIndex(3);
            if (drawCustomDragFloat("Z", "##rotZ", ImGui::GetCursorScreenPos(), SIZE, ROUNDING, 50.0f, blue, white, &rotZ, 1.0f)) {
                rotation.z = rotZ;
            }
        }

        if (displayScale)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Scale");

            ImGui::TableSetColumnIndex(1);
            if (drawCustomDragFloat("X", "##scaX", ImGui::GetCursorScreenPos(), SIZE, ROUNDING, 50.0f, green, white, &scaX, 0.01f)) {
                scale.x = scaX;
            }
            
            ImGui::TableSetColumnIndex(2);
            if (drawCustomDragFloat("Y", "##scaY", ImGui::GetCursorScreenPos(), SIZE, ROUNDING, 50.0f, red, white, &scaY, 0.01f)) {
                scale.y = scaY;
            }
            
            ImGui::TableSetColumnIndex(3);
            if (drawCustomDragFloat("Z", "##scaZ", ImGui::GetCursorScreenPos(), SIZE, ROUNDING, 50.0f, blue, white, &scaZ, 0.01f)) {
                scale.z = scaZ;
            }
        }

        ImGui::EndTable();
    }

    if (cameraComponent)
    {
        cameraComponent->getCamera()->position = position;
    }
    else if (lightComponent)
    {
        lightComponent->getLight()->position = position;
    }
    else if (primitiveComponent)
    {
        auto p = primitiveComponent->getPrimitive();
        p->position = position;
        p->rotation = rotation;
        p->scale = scale;
    }
    else if (modelComponent)
    {
        auto p = modelComponent->getModel();
        p->position = position;
        p->rotation = rotation;
        p->scale = scale;
    }

    updateTransformComponent(transformComponent, position, rotation, scale); // dirty
}


void engine::ImGuiEditor::renderLightComponent(std::shared_ptr<LightComponent>& component)
{
    ImGui::SeparatorText(component->getName().c_str());
    
    if (!component)
        return;

    std::shared_ptr<PointLight> pointLight;
    std::shared_ptr<DirectionalLight> dirLight;
    std::shared_ptr<SpotLight> spotLight;

    if (pointLight = std::dynamic_pointer_cast<PointLight>(component->getLight()))
    {
    }
    else if (dirLight = std::dynamic_pointer_cast<DirectionalLight>(component->getLight()))
    {
    }
    else if (spotLight = std::dynamic_pointer_cast<SpotLight>(component->getLight()))
    {
    }

    if (spotLight)
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
    else if (dirLight)
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
            ImGui::DragFloat("##targetX", &dirLight->target.x, 0.1f);

            ImGui::TableSetColumnIndex(3);
            ImGui::Text("Y");
            ImGui::TableSetColumnIndex(4);
            ImGui::DragFloat("##targetY", &dirLight->target.y, 0.1f);

            ImGui::TableSetColumnIndex(5);
            ImGui::Text("Z");
            ImGui::TableSetColumnIndex(6);
            ImGui::DragFloat("##targetZ", &dirLight->target.z, 0.1f);

            ImGui::EndTable();
        }
    }

    // common to all lights
    if (ImGui::BeginTable("MyTable", 2, ImGuiTableFlags_SizingStretchSame))
    {
        ImGui::TableSetupColumn("Labels", ImGuiTableColumnFlags_WidthFixed, itemLabelWidth);
        ImGui::TableSetupColumn("Controls", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Intensity");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(80);
        ImGui::DragFloat("##intensity", &component->getLight()->intensity, 1.0f, 0.0f, 1000.0f, "%.3f", ImGuiSliderFlags_None);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Ambient Color");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-FLT_MIN); // Use all available width
        ImGui::ColorEdit3("##ambientColor", glm::value_ptr(component->getLight()->ambientColor), ImGuiColorEditFlags_NoLabel);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Diffuse Color");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-FLT_MIN); // Use all available width
        ImGui::ColorEdit3("##diffuseColor", glm::value_ptr(component->getLight()->diffuseColor), ImGuiColorEditFlags_NoLabel);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Specular Color");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-FLT_MIN); // Use all available width
        // Show color preview
        ImGui::ColorEdit3("##specularColor", glm::value_ptr(component->getLight()->specularColor), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::SameLine();

        // Show numeric inputs without labels
        ImGui::DragFloat3(
            "##specularColorInputs",
            glm::value_ptr(component->getLight()->specularColor),
            0.01f,
            0.0f,
            1.0f
        );

        ImGui::EndTable();
    }
}

void engine::ImGuiEditor::renderCameraComponent(std::shared_ptr<CameraComponent>& component)
{
    ImGui::SeparatorText(component->getName().c_str());
    
    if (!component)
        return;

    // Use a temporary variable to hold the zoom value
    float zoom = component->getCamera()->zoom;
    float yaw = component->getCamera()->yaw;
    float pitch = component->getCamera()->pitch;

    if (ImGui::BeginTable("MyTable", 2, ImGuiTableFlags_SizingStretchSame))
    {
        ImGui::TableSetupColumn("Labels", ImGuiTableColumnFlags_WidthFixed, itemLabelWidth);
        ImGui::TableSetupColumn("Controls", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Zoom");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(80);
        if (ImGui::DragFloat("##zoom", &zoom, 1.0f, 0.0f, 1000.0f, "%.3f", ImGuiSliderFlags_None))
        {
            component->getCamera()->zoom = zoom;
        }


        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Yaw");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(80);
        if (ImGui::DragFloat("##yaw", &yaw, 5.0f, -180.0f, 180.0f, "%.0f", ImGuiSliderFlags_None))
        {
            component->getCamera()->yaw = yaw;
        }


        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Pitch");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(80);
        if (ImGui::DragFloat("##pitch", &pitch, 5.0f, -180.0f, 180.0f, "%.0f", ImGuiSliderFlags_None))
        {
            component->getCamera()->pitch = pitch;
        }

        ImGui::EndTable();
    }
}

void engine::ImGuiEditor::renderPrimitiveComponent(std::shared_ptr<PrimitiveComponent>& component, std::shared_ptr<TransformComponent>& transformComponent)
{
    ImGui::SeparatorText(component->getName().c_str());

    if (!component)
        return;


    auto primitive = component->getPrimitive();
    if (!primitive)
		return;


    // draw component properties dynamically
    auto properties = component->getPublicProperties();
    auto componentType = to_string(component->getPrimitive()->getTypeID());
    auto componentName = component->getName();

    if (ImGui::BeginTable("MyTable", 2, ImGuiTableFlags_SizingStretchSame))
    {
        ImGui::TableSetupColumn("Labels", ImGuiTableColumnFlags_WidthFixed, itemLabelWidth);
        ImGui::TableSetupColumn("Controls", ImGuiTableColumnFlags_WidthStretch);
        properties.forEach([&](const auto& key, auto& value)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text(key.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(80);
                if (auto pValue = std::get_if<float>(&value))
                {
                    if (ImGui::DragFloat(std::format("##{}{}{}", componentName, componentType, key).c_str(), pValue, 0.01f, 0.0f, 10.0f, "%.3f", ImGuiSliderFlags_NoRoundToFormat))
                    {
                        // float value changed
                        component->setProperty(key, *pValue);
                    }
                }
                else if (auto pValue = std::get_if<bool>(&value))
                {
                    if (ImGui::Checkbox(std::format("##{}{}{}", componentName, componentType, key).c_str(), pValue))
                    {
                        // bool value changed
                        component->setProperty(key, *pValue);
                    }
                }
            });
        ImGui::EndTable();
    }
}

void engine::ImGuiEditor::updateTransformComponent(std::shared_ptr<TransformComponent>& transformComponent, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
{
    auto trs = transformComponent->getTransform();
    trs.setLocalPosition(position);
    trs.setLocalRotation(rotation);
    trs.setLocalScale(scale);
    transformComponent->setTransform(trs);
}

void engine::ImGuiEditor::renderModelComponent(std::shared_ptr<ModelComponent>& component)
{
    ImGui::SeparatorText(component->getName().c_str());

    if (!component)
        return;
}

GLuint engine::ImGuiEditor::getEntityTypeSmallIcon(const engine::EntityType entityType)
{
    auto it = m_iconSmallTextureCache.find(entityType);
    if (it != m_iconSmallTextureCache.end())
    {
        return it->second;
    }
    else
    {
        auto iconName = std::format("icon_{}_16x16.png", static_cast<int>(entityType));
        GLuint iconTexture = Texture::loadGLTextureFromFile(iconName.c_str(), "icons");

        m_iconSmallTextureCache.insert(std::make_pair(entityType, iconTexture));

        return iconTexture;
    }
}

GLuint engine::ImGuiEditor::getEntityTypeMediumIcon(const engine::EntityType entityType)
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

ImVec4 engine::ImGuiEditor::getEntityColor(const engine::EntityType entityType)
{
    if (entityType == engine::EntityType::model)
    {
        auto purple = engine::Colors::hexToNormalizedRGB("#d478ff");
        return ImVec4(purple.r, purple.g, purple.b, purple.a);
    }
    else if (entityType == engine::EntityType::primitive)
    {
        auto green = engine::Colors::hexToNormalizedRGB("#abff78");
        return ImVec4(green.r, green.g, green.b, green.a);
    }
    else if (entityType == engine::EntityType::light)
    {
        auto yellow = engine::Colors::hexToNormalizedRGB("#ffd83b");
        return ImVec4(yellow.r, yellow.g, yellow.b, yellow.a);
    }
    else if (entityType == engine::EntityType::camera)
    {
        auto blue = engine::Colors::hexToNormalizedRGB("#0f9cff");
        return ImVec4(blue.r, blue.g, blue.b, blue.a);
    }

    return ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
}

GLuint engine::ImGuiEditor::getEntityActionIcon(const std::string& key)
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


void engine::ImGuiEditor::drawCustomLabel(const char* text, const ImVec2& position, const ImVec2& size, float rounding, ImU32 backgroundColor, ImU32 foregroundColor)
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


bool engine::ImGuiEditor::drawCustomDragFloat(const char* text, const char* name, const ImVec2& position, const ImVec2& size, float rounding, float width, ImU32 backgroundColor, ImU32 foregroundColor, float* value, float step)
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
    bool res = ImGui::DragFloat(name, value, step, 0.0f, 0.0f, "%.2f");
    ImGui::PopStyleColor(3);

    return res;
}
#endif
