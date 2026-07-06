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





#include "../../include/managers/entity_manager.h"

// https://github.com/TheCherno/ImGuizmo
#include "extensions/imGuizmo/ImGuizmo.h"



#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <imgui_internal.h>

#include <unordered_map>

#if EDITOR_MODE


void engine::ImGuiEditor::init()
{
    EditorHelper::registerIconAtlas();

    m_vramManager.init();
}

void engine::ImGuiEditor::setScene(std::shared_ptr<Entity> rootEntity)
{
    m_rootEntity = rootEntity;
    m_selectedEntity = rootEntity;
}

/// <summary>
/// https://github.com/ocornut/imgui/issues/2109#issuecomment-430096134
/// </summary>
/// <param name="show"></param>
void engine::ImGuiEditor::renderUIWindow(bool show, glm::mat4& projection, glm::mat4& view, const bool displayObjectTransformGuizmo)
{
	bool open = true;

	static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    dockspace_flags |= ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 5.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
	ImGui::Begin("DockSpace", &open, window_flags);
	ImGui::PopStyleVar(3);

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
        ImGuiID dock_id_top = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.10f, NULL, &dock_main_id);

		ImGui::DockBuilderDockWindow("Scene", dock_id_left);
        ImGui::DockBuilderDockWindow("Settings", dock_id_left);
		ImGui::DockBuilderDockWindow("About", dock_id_left);

		ImGui::DockBuilderDockWindow("Properties", dock_id_right);

        ImGui::DockBuilderDockWindow("FloatingToolbar", dock_id_top);

		ImGui::DockBuilderFinish(dockspace_id);
	}

	// custom docked windows tab height and padding
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 5.0f));

	ImGui::DockSpace(dockspace_id, ImGui::GetContentRegionAvail(), dockspace_flags);

	ImGui::End();

	ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_None);
    renderHierarchyWidget();
	ImGui::End();

    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_None);
    renderTabSettings();
    ImGui::End();

	ImGui::Begin("About", nullptr, ImGuiWindowFlags_None);
    renderTabAbout();
	ImGui::End();

	ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_None);
    renderPropertiesWidget();
	ImGui::End();

    ImGui::PopStyleVar();

    renderGuizmo(dockspace_id, projection, view, displayObjectTransformGuizmo);


    TestOverlay();
}

void engine::ImGuiEditor::initRenderGuizmo(const std::shared_ptr<Camera> camera)
{
    m_guizmoCamera = camera;
    camDistance = camera->getDistanceToTarget(glm::vec3(0.0f, -0.35f, 0.0f));
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
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 2.0f));
        ImGui::BeginChild("EntityPropertyRegion", ImVec2(0, 0), true, ImGuiWindowFlags_None);
        displayEntityDetails(m_selectedEntity);
        ImGui::EndChild();
        ImGui::PopStyleVar(3); // Restore default
    }
}

void engine::ImGuiEditor::renderTabSettings()
{
    ImGui::BeginChild("SettingsRegion", ImVec2(0, 0), true);

    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    ImGuiTogglePalette material_palette_on;
    material_palette_on.Frame = EditorHelper::im_dark;
    material_palette_on.Knob = EditorHelper::im_white;
    material_palette_on.KnobHover = EditorHelper::im_white;
    material_palette_on.FrameBorder = EditorHelper::im_light;

    ImGuiTogglePalette material_palette_off;
    material_palette_off.Frame = EditorHelper::im_dark;
    material_palette_off.Knob = EditorHelper::im_gray;
    material_palette_off.KnobHover = EditorHelper::im_gray;
    material_palette_off.FrameBorder = EditorHelper::im_light;

    ImGuiToggleConfig toggle_config;
    toggle_config.Flags |= ImGuiToggleFlags_Bordered | ImGuiToggleFlags_Animated;
    toggle_config.Size = ImVec2(30.0f, 18.0f);
    toggle_config.On.Palette = &material_palette_on;
    toggle_config.Off.Palette = &material_palette_off;

    ImGui::Spacing();


    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));


    // ------------------------------------------------------------------------
    // Rendering params
    // ------------------------------------------------------------------------

    static int lastRenderMethod = static_cast<int>(DEFAULT_RENDER_METHOD);
    EditorHelper::renderSliderIntWithLabel("Render method", "render_method", sceneSetting_renderMethod, lastRenderMethod, 1, 2, m_onSceneSettingChanged);

    static bool lastDrawWireframe = DEFAULT_ENABLE_WIREFRAME_MODE;
    if (ImGui::Toggle("Wireframe", &sceneSetting_drawAsWireframe, toggle_config))
    {
        if (m_onSceneSettingChanged && lastDrawWireframe != sceneSetting_drawAsWireframe)
        {
            m_onSceneSettingChanged("draw_wireframe", sceneSetting_drawAsWireframe);
            lastDrawWireframe = sceneSetting_drawAsWireframe;
        }
    }

    static ubyte lastFrameBufferMsaaSamples = DEFAULT_FRAMEBUFFER_MSAA_SAMPLES;
    EditorHelper::renderSliderUnsignedByteWithLabel("Framebuffer antialiasing samples", "framebuffer_msaa_samples", sceneSetting_framebufferMsaaSamples, lastFrameBufferMsaaSamples, 0, 16, m_onSceneSettingChanged);

    static bool lastEnableGammaCorection = DEFAULT_ENABLE_GAMMA_CORRECTION;
    if (ImGui::Toggle("Gamma correction", &sceneSetting_enableGammaCorrection, toggle_config))
    {
        if (m_onSceneSettingChanged && lastEnableGammaCorection != sceneSetting_enableGammaCorrection)
        {
            m_onSceneSettingChanged("enable_gamma_correction", sceneSetting_enableGammaCorrection);
            lastEnableGammaCorection = sceneSetting_enableGammaCorrection;
        }
    }

    static bool lastEnableToneMapping = DEFAULT_ENABLE_TONE_MAPPING;
    if (ImGui::Toggle("Tone mapping", &sceneSetting_enableToneMapping, toggle_config))
    {
        if (m_onSceneSettingChanged && lastEnableToneMapping != sceneSetting_enableToneMapping)
        {
            m_onSceneSettingChanged("enable_tone_mapping", sceneSetting_enableToneMapping);
            lastEnableToneMapping = sceneSetting_enableToneMapping;
        }
    }

    static float lastExposure = DEFAULT_EXPOSURE;
    EditorHelper::renderDragFloatWithLabel("Exposure", "exposure", sceneSetting_exposure, lastExposure, 0.0f, 5.0f, 0.1f, "%.1f", m_onSceneSettingChanged);


    static int lastApplyPostProcessFx = DEFAULT_POST_PROCESSING_FX;
    EditorHelper::renderSliderIntWithLabel("Post process", "post_process", sceneSetting_applyPostProcessFx, lastApplyPostProcessFx, 0, 10, m_onSceneSettingChanged);

    // ------------------------------------------------------------------------
    // optims
    // ------------------------------------------------------------------------

    static bool lastEnableFaceCulling = DEFAULT_ENABLE_FACE_CULLING;
    if (ImGui::Toggle("Face culling", &sceneSetting_enableFaceCulling, toggle_config))
    {
        if (m_onSceneSettingChanged && lastEnableFaceCulling != sceneSetting_enableFaceCulling)
        {
            m_onSceneSettingChanged("enable_face_culling", sceneSetting_enableFaceCulling);
			lastEnableFaceCulling = sceneSetting_enableFaceCulling;
        }
    }

    static bool lastEnableCameraFrustrumCulling = DEFAULT_ENABLE_CAMERA_FRUSTRUM_CULLING;
    if (ImGui::Toggle("Camera frustrum culling", &sceneSetting_enableCameraFrustrumCulling, toggle_config))
    {
        if (m_onSceneSettingChanged && lastEnableCameraFrustrumCulling != sceneSetting_enableCameraFrustrumCulling)
        {
            m_onSceneSettingChanged("enable_camera_frustrum_culling", sceneSetting_enableCameraFrustrumCulling);
            lastEnableCameraFrustrumCulling = sceneSetting_enableCameraFrustrumCulling;
        }
    }

    // ------------------------------------------------------------------------
    // Visual helpers
    // ------------------------------------------------------------------------

    static bool lastDrawLightsVisualHelpers = DEFAULT_ENABLE_LIGHTS_VISUAL_HELPERS;
    if (ImGui::Toggle("Lights visual helpers", &sceneSetting_drawLightsVisualHelpers, toggle_config))
    {
        if (m_onSceneSettingChanged && lastDrawLightsVisualHelpers != sceneSetting_drawLightsVisualHelpers)
        {
            m_onSceneSettingChanged("draw_lights_visual_helpers", sceneSetting_drawLightsVisualHelpers);
            lastDrawLightsVisualHelpers = sceneSetting_drawLightsVisualHelpers;
        }
    }

    static bool lastDrawBoundingBoxesVisualHelpers = DEFAULT_ENABLE_BOUNDINGBOX_VISUAL_HELPERS;
    if (ImGui::Toggle("Bounding boxes visual helpers", &sceneSetting_drawBoundingBoxesVisualHelpers, toggle_config))
    {
        if (m_onSceneSettingChanged && lastDrawBoundingBoxesVisualHelpers != sceneSetting_drawBoundingBoxesVisualHelpers)
        {
            m_onSceneSettingChanged("draw_bounding_boxes_visual_helpers", sceneSetting_drawBoundingBoxesVisualHelpers);
            lastDrawBoundingBoxesVisualHelpers = sceneSetting_drawBoundingBoxesVisualHelpers;
        }
    }

    static bool lastDrawDebugNormalsVisualHelpers = DEFAULT_ENABLE_NORMALS_VISUAL_HELPERS;
    if (ImGui::Toggle("Normals visual helpers", &sceneSetting_drawDebugNormalsVisualHelpers, toggle_config))
    {
        if (m_onSceneSettingChanged && lastDrawDebugNormalsVisualHelpers != sceneSetting_drawDebugNormalsVisualHelpers)
        {
            m_onSceneSettingChanged("draw_debug_normals_visual_helpers", sceneSetting_drawDebugNormalsVisualHelpers);
            lastDrawDebugNormalsVisualHelpers = sceneSetting_drawDebugNormalsVisualHelpers;
        }
    }

    // ------------------------------------------------------------------------
    // shadows
    // ------------------------------------------------------------------------

    static bool lastEnableShadows = DEFAULT_ENABLE_SHADOWS;
    if (ImGui::Toggle("Enable shadows", &sceneSetting_enableShadows, toggle_config))
    {
        if (m_onSceneSettingChanged && lastEnableShadows != sceneSetting_enableShadows)
        {
            m_onSceneSettingChanged("enable_shadows", sceneSetting_enableShadows);
            lastEnableShadows = sceneSetting_enableShadows;
        }
    }

    static int lastShadowCalculationMethod = static_cast<int>(DEFAULT_SHADOWS_METHOD);
    EditorHelper::renderSliderIntWithLabel("Shadow maps method", "shadow_calculation_method", sceneSetting_shadowCalculationMethod, lastShadowCalculationMethod, 1, 3, m_onSceneSettingChanged);

    static int lastShadowMapTextureSize = static_cast<int>(DEFAULT_SHADOWMAP_TEXTURE_SIZE);
    EditorHelper::renderSliderIntWithLabel("Shadow maps texture size", "shadow_maps_texture_size", sceneSetting_shadowMapTextureSize, lastShadowMapTextureSize, 256, 4096, m_onSceneSettingChanged);

    static float lastShadowIntensity = DEFAULT_SHADOWS_INTENSITY;
    EditorHelper::renderDragFloatWithLabel("Shadow maps intensity", "shadow_intensity", sceneSetting_shadowIntensity, lastShadowIntensity, 0.0f, 5.0f, 0.1f, "%.1f", m_onSceneSettingChanged);

    // Typical values(tune slightly per scene)
    // material.shadowMapsBias = 0.02;   // indoor
    // material.shadowMapsBias = 0.05;   // outdoor / large scenes
    static float lastShadowMapsBiasFactor = DEFAULT_SHADOW_MAPS_BIAS;
    EditorHelper::renderDragFloatWithLabel("Shadow maps bias", "shadow_maps_bias_factor", sceneSetting_shadowMapBiasFactor, lastShadowMapsBiasFactor, 0.001f, 0.05f, 0.001f, "%.3f", m_onSceneSettingChanged);

    static float lastShadowMapsBlur = DEFAULT_SHADOWS_BLUR;
    EditorHelper::renderDragFloatWithLabel("Shadow maps blur", "shadow_maps_blur_factor", sceneSetting_shadowMapBlur, lastShadowMapsBlur, 0.0f, 50.0f, 0.1f, "%.1f", m_onSceneSettingChanged);


    // ------------------------------------------------------------------------
    // PBR renderer only !!!!
    // ------------------------------------------------------------------------
    
    if (sceneSetting_renderMethod == static_cast<int>(RenderMethod::PBR))
    {
        static float lastIblDiffuseIntensity = DEFAULT_PBR_IBL_DIFFUSE_INTENSITY;
        EditorHelper::renderDragFloatWithLabel("IBL Diffuse Intensity", "pbr_ibl_diffuse_intensity", sceneSetting_iblDiffuseIntensity, lastIblDiffuseIntensity, 0.0f, 10.0f, 0.1f, "%.1f", m_onSceneSettingChanged);

        static float lastIblSpecularIntensity = DEFAULT_PBR_IBL_SPECULAR_INTENSITY;
        EditorHelper::renderDragFloatWithLabel("IBL Specular Intensity", "pbr_ibl_specular_intensity", sceneSetting_iblSpecularIntensity, lastIblSpecularIntensity, 0.0f, 10.0f, 0.1f, "%.1f", m_onSceneSettingChanged);
    }


    

    ImGui::PopStyleVar();

    ImGui::PopStyleColor();

    ImGui::EndChild();
}

void engine::ImGuiEditor::renderTabAbout()
{
    auto vramInfo = m_vramManager.query();
    
    ImGui::BeginChild("AboutRegion", ImVec2(0, 0), true);
    ImGui::Text("GPU Vendor:\n%s", m_sysMonitor.GetGPUVendor().c_str());
    
    double total = vramInfo.totalBytes / (1024.0 * 1024.0);
    ImGui::Text("GPU VRAM Total: %.0f MB", total);

    double used = vramInfo.usedBytes / (1024.0 * 1024.0);
    double free = vramInfo.freeBytes / (1024.0 * 1024.0);
    ImGui::Text("GPU VRAM Used: %.0f MB / Free: %.0f MB", used, free);
    
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

    GLuint tex = EditorHelper::getIconAtlasTexture();
    IM_ASSERT(tex != 0);

    // Draw the icon
    auto uvEntityType = EditorHelper::getEntityTypeSmallIcon(convertEntityTypeToAtlasIcon(entityType, 16));
    ImGui::Image((ImTextureID)(intptr_t)tex, ImVec2(16, 16), uvEntityType.uv0, uvEntityType.uv1);

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

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 0.f));
    
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(-2.f, 0.f));

    // Draw the show/hide entity button
    auto uvShowHide = EditorHelper::getIcon(entity->enabled ? EditorIcon::show : EditorIcon::hide);
    if (ImGui::ImageButton("##visible", (ImTextureID)(intptr_t)tex, ImVec2(16, 16), uvShowHide.uv0, uvShowHide.uv1))
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

engine::EditorIcon engine::ImGuiEditor::convertEntityTypeToAtlasIcon(const engine::EntityType type, unsigned int Iconsize) const
{
    if (Iconsize == 16)
    {
        switch (type)
        {
        case EntityType::model:
            return EditorIcon::entity_model_16x16;
        case EntityType::primitive:
            return EditorIcon::entity_primitive_16x16;
        case EntityType::light:
            return EditorIcon::entity_light_16x16;
        case EntityType::camera:
            return EditorIcon::entity_camera_16x16;
        case EntityType::particleSystem:
            return EditorIcon::entity_particleSystem_16x16;
        case EntityType::terrain:
            return EditorIcon::entity_terrain_16x16;
        }
    }
    else if (Iconsize == 48)
    {
        switch (type)
        {
        case EntityType::model:
            return EditorIcon::entity_model_48x48;
        case EntityType::primitive:
            return EditorIcon::entity_primitive_48x48;
        case EntityType::light:
            return EditorIcon::entity_light_48x48;
        case EntityType::camera:
            return EditorIcon::entity_camera_48x48;
        case EntityType::particleSystem:
            return EditorIcon::entity_particleSystem_48x48;
        case EntityType::terrain:
            return EditorIcon::entity_terrain_48x48;
        }
    }

    return EditorIcon::undefined;
}

void engine::ImGuiEditor::displayEntityDetails(const std::shared_ptr<Entity>& entity)
{
    if (entity)
    {
        auto entityType = entity->getType();
        auto uv = EditorHelper::getEntityTypeMediumIcon(convertEntityTypeToAtlasIcon(entityType, 48));
        GLuint tex = EditorHelper::getIconAtlasTexture();

        IM_ASSERT(tex != 0);

        // Draw the icon
        ImGui::Image((ImTextureID)(intptr_t)tex, ImVec2(48, 48), uv.uv0, uv.uv1);

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
            renderTransformComponent(entity);
        }
        else if (typeID == ComponentType::camera)
        {
            // camera component
            auto cameraComponent = dynamic_pointer_cast<CameraComponent>(component);
            if (cameraComponent) renderCameraComponent(cameraComponent);
        }
        else if (typeID == ComponentType::light)
        {
            // light component
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
            if (primitiveComponent) renderPrimitiveComponent(primitiveComponent);
        }
        else if (typeID == ComponentType::animator)
        {
            // animator component
            auto animatorComponent = dynamic_pointer_cast<AnimatorComponent>(component);
            if (animatorComponent) renderAnimatorComponent(animatorComponent);
        }
        else if (typeID == ComponentType::particleSystem)
        {
            // particle system component
            auto particleSystemComponent = dynamic_pointer_cast<ParticleSystemComponent>(component);
            if (particleSystemComponent) renderParticleSystemComponent(particleSystemComponent);
        }
        else if (typeID == ComponentType::terrain)
        {
            // terrain component
            auto terrainComponent = dynamic_pointer_cast<TerrainComponent>(component);
            if (terrainComponent) renderTerrainComponent(terrainComponent);
        }
    }
}


void engine::ImGuiEditor::renderTransformComponent(const std::shared_ptr<Entity>& entity)
{
    auto transformComponent = entity->getComponent<TransformComponent>();

    bool displayPosition{ true };
    bool displayRotation{ true };
    bool displayScale{ true };

    if (!transformComponent)
        return;

    //static bool isHeaderChecked = true;
    static bool isHeaderExpanded = true; // Set to true to start expanded

    bool enabled = transformComponent->isEnabled();

    std::function<void(bool)> onCheck = [transformComponent, &enabled](bool checked) {
        transformComponent->setEnabled(checked);
        enabled = transformComponent->isEnabled();
        };


    ImGui::SetNextItemOpen(isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::collapsingCheckboxHeader(transformComponent->getName().c_str(), &enabled, ImGuiTreeNodeFlags_None, onCheck))
    {
        glm::vec3 position{ 0,0,0 };
        glm::vec3 rotation{ 0,0,0 };
        glm::vec3 scale{ 1,1,1 };

        std::shared_ptr<CameraComponent> cameraComponent{};
        std::shared_ptr<LightComponent> lightComponent{};
        std::shared_ptr<PrimitiveComponent> primitiveComponent{};
        std::shared_ptr<ModelComponent> modelComponent{};
        std::shared_ptr<ParticleSystemComponent> particleSystemComponent{};
        std::shared_ptr<TerrainComponent> terrainComponent{};

        if (cameraComponent = entity->getComponent<CameraComponent>())
        {
            position = cameraComponent->getCamera()->position;
            displayRotation = false;
            displayScale = false;
        }
        else if (lightComponent = entity->getComponent<LightComponent>())
        {
            position = lightComponent->getLight()->getPosition();
            displayRotation = false;
            displayScale = false;
        }
        else if (primitiveComponent = entity->getComponent<PrimitiveComponent>())
        {
            position = primitiveComponent->getPrimitive()->getPosition();
            scale = primitiveComponent->getPrimitive()->getScale();
            rotation = primitiveComponent->getPrimitive()->getRotation();
        }
        else if (modelComponent = entity->getComponent<ModelComponent>())
        {
            position = modelComponent->getModel()->getPosition();
            scale = modelComponent->getModel()->getScale();
            rotation = modelComponent->getModel()->getRotation();
        }
        else if (particleSystemComponent = entity->getComponent<ParticleSystemComponent>())
        {
            position = particleSystemComponent->getParticleSystem()->getPosition();
            scale = particleSystemComponent->getParticleSystem()->getScale();
            rotation = particleSystemComponent->getParticleSystem()->getRotation();
        }
        else if (terrainComponent = entity->getComponent<TerrainComponent>())
        {
            position = terrainComponent->getTerrain()->getPosition();
            scale = terrainComponent->getTerrain()->getScale();
            rotation = terrainComponent->getTerrain()->getRotation();
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
            ImGui::TableSetupColumn("Labels", ImGuiTableColumnFlags_WidthFixed, EditorHelper::ITEM_LABEL_WIDTH);
            ImGui::TableSetupColumn("vx", ImGuiTableColumnFlags_WidthFixed, 74.0f);
            ImGui::TableSetupColumn("vy", ImGuiTableColumnFlags_WidthFixed, 74.0f);
            ImGui::TableSetupColumn("vz", ImGuiTableColumnFlags_WidthFixed, 74.0f);

            if (displayPosition)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Position");

                ImGui::TableSetColumnIndex(1);
                if (EditorHelper::drawCustomDragFloat("X", "##posX", ImGui::GetCursorScreenPos(), EditorHelper::SIZE, EditorHelper::ROUNDING, 50.0f, EditorHelper::green, EditorHelper::white, &posX, 0.01f)) {
                    position.x = posX;
                }

                ImGui::TableSetColumnIndex(2);
                if (EditorHelper::drawCustomDragFloat("Y", "##posY", ImGui::GetCursorScreenPos(), EditorHelper::SIZE, EditorHelper::ROUNDING, 50.0f, EditorHelper::red, EditorHelper::white, &posY, 0.01f)) {
                    position.y = posY;
                }

                ImGui::TableSetColumnIndex(3);
                if (EditorHelper::drawCustomDragFloat("Z", "##posZ", ImGui::GetCursorScreenPos(), EditorHelper::SIZE, EditorHelper::ROUNDING, 50.0f, EditorHelper::blue, EditorHelper::white, &posZ, 0.01f)) {
                    position.z = posZ;
                }
            }

            if (displayRotation)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Rotation");

                ImGui::TableSetColumnIndex(1);
                if (EditorHelper::drawCustomDragFloat("X", "##rotX", ImGui::GetCursorScreenPos(), EditorHelper::SIZE, EditorHelper::ROUNDING, 50.0f, EditorHelper::green, EditorHelper::white, &rotX, 1.0f)) {
                    rotation.x = rotX;
                }

                ImGui::TableSetColumnIndex(2);
                if (EditorHelper::drawCustomDragFloat("Y", "##rotY", ImGui::GetCursorScreenPos(), EditorHelper::SIZE, EditorHelper::ROUNDING, 50.0f, EditorHelper::red, EditorHelper::white, &rotY, 1.0f)) {
                    rotation.y = rotY;
                }

                ImGui::TableSetColumnIndex(3);
                if (EditorHelper::drawCustomDragFloat("Z", "##rotZ", ImGui::GetCursorScreenPos(), EditorHelper::SIZE, EditorHelper::ROUNDING, 50.0f, EditorHelper::blue, EditorHelper::white, &rotZ, 1.0f)) {
                    rotation.z = rotZ;
                }
            }

            if (displayScale)
            {
                ImGui::TableNextRow();

				static bool scaleLinked = false;

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Scale");
                ImGui::SameLine(98.0f); // align to right side
                EditorHelper::addDiscreetIconButton(scaleLinked, "LockUnlock", EditorIcon::unlocked, EditorIcon::locked, []() {});

                ImGui::TableSetColumnIndex(1);
                if (EditorHelper::drawCustomDragFloat("X", "##scaX", ImGui::GetCursorScreenPos(), EditorHelper::SIZE, EditorHelper::ROUNDING, 50.0f, EditorHelper::green, EditorHelper::white, &scaX, 0.01f)) {
                    if (!scaleLinked)
                    {
                        scale.x = scaX;
                    }
                    else
                    {
                        scale.x = scaX;
                        scale.y = scaX;
                        scale.z = scaX;
                    }
                }

                ImGui::TableSetColumnIndex(2);
                if (EditorHelper::drawCustomDragFloat("Y", "##scaY", ImGui::GetCursorScreenPos(), EditorHelper::SIZE, EditorHelper::ROUNDING, 50.0f, EditorHelper::red, EditorHelper::white, &scaY, 0.01f)) {
                    if (!scaleLinked)
                    {
                        scale.y = scaY;
                    }
                    else
                    {
                        scale.y = scaY;
                        scale.x = scaY;
                        scale.z = scaY;
					}
                }

                ImGui::TableSetColumnIndex(3);
                if (EditorHelper::drawCustomDragFloat("Z", "##scaZ", ImGui::GetCursorScreenPos(), EditorHelper::SIZE, EditorHelper::ROUNDING, 50.0f, EditorHelper::blue, EditorHelper::white, &scaZ, 0.01f)) {
                    if (!scaleLinked)
                    {
                        scale.z = scaZ;
                    }
                    else
                    {
                        scale.z = scaZ;
                        scale.x = scaZ;
                        scale.y = scaZ;
                    }
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
            lightComponent->getLight()->setPosition(position);
        }
        else if (primitiveComponent)
        {
            auto p = primitiveComponent->getPrimitive();
            p->setPosition(position);
            p->setRotation(rotation);
            p->setScale(scale);
        }
        else if (modelComponent)
        {
            auto p = modelComponent->getModel();
            p->setPosition(position);
            p->setRotation(rotation);
            p->setScale(scale);
        }
        else if (particleSystemComponent)
        {
            auto p = particleSystemComponent->getParticleSystem();
            p->setPosition(position);
            p->setRotation(rotation);
            p->setScale(scale);
        }
        else if (terrainComponent)
        {
            auto p = terrainComponent->getTerrain();
            p->setPosition(position);
            p->setRotation(rotation);
            p->setScale(scale);
        }

        updateTransformComponent(transformComponent, position, rotation, scale); // dirty
    }
}


void engine::ImGuiEditor::renderLightComponent(std::shared_ptr<LightComponent>& component)
{
    auto light = component->getLight();
    if (!light)
        return;


    bool enabled = component->isEnabled();

    std::function<void(bool)> onCheck = [component, &enabled](bool checked) {
        component->setEnabled(checked);
        enabled = component->isEnabled();
        };

    static bool isHeaderExpanded = true; // Set to true to start expanded

    ImGui::SetNextItemOpen(isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::collapsingCheckboxHeader(component->getName().c_str(), &enabled, ImGuiTreeNodeFlags_None, onCheck))
    {
        EditorHelper::renderDynamicProperties(component, to_string(light->getTypeID()));
    }
}

void engine::ImGuiEditor::renderCameraComponent(std::shared_ptr<CameraComponent>& component)
{
    auto camera = component->getCamera();
    if (!camera)
        return;

    bool enabled = component->isEnabled();

    std::function<void(bool)> onCheck = [component, &enabled](bool checked) {
        component->setEnabled(checked);
        enabled = component->isEnabled();
        };

    static bool isHeaderExpanded = true; // Set to true to start expanded

    ImGui::SetNextItemOpen(isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::
        collapsingCheckboxHeader(component->getName().c_str(), &enabled, ImGuiTreeNodeFlags_None, onCheck))
    {
        EditorHelper::renderDynamicProperties(component, to_string(camera->getTypeID()));
    }
}

void engine::ImGuiEditor::renderPrimitiveComponent(std::shared_ptr<PrimitiveComponent>& component)
{
    auto primitive = component->getPrimitive();
    if (!primitive)
        return;

    bool enabled = component->isEnabled();

    std::function<void(bool)> onCheck = [component, &enabled](bool checked) {
        component->setEnabled(checked);
        enabled = component->isEnabled();
        };

    static bool isHeaderExpanded = true; // Set to true to start expanded

    ImGui::SetNextItemOpen(isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::collapsingCheckboxHeader(component->getName().c_str(), &enabled, ImGuiTreeNodeFlags_None, onCheck))
    {
        EditorHelper::renderDynamicProperties(component, to_string(primitive->getTypeID()));
    }
}

void engine::ImGuiEditor::renderAnimatorComponent(std::shared_ptr<AnimatorComponent>& component)
{
    auto animator = component->getAnimator();
    if (!animator)
        return;

    bool enabled = component->isEnabled();

    std::function<void(bool)> onCheck = [component, &enabled](bool checked) {
        component->setEnabled(checked);
        enabled = component->isEnabled();
        };

    static bool isHeaderExpanded = true; // Set to true to start expanded

    ImGui::SetNextItemOpen(isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::collapsingCheckboxHeader(component->getName().c_str(), &enabled, ImGuiTreeNodeFlags_None, onCheck))
    {
        EditorHelper::renderDynamicProperties(component, to_string(animator->getTypeID()));
    }
}

void engine::ImGuiEditor::renderParticleSystemComponent(std::shared_ptr<ParticleSystemComponent>& component)
{
    auto particleSystem = component->getParticleSystem();
    if (!particleSystem)
        return;

    bool enabled = component->isEnabled();

    std::function<void(bool)> onCheck = [component, &enabled](bool checked) {
        component->setEnabled(checked);
        enabled = component->isEnabled();
        };

    static bool isHeaderExpanded = true; // Set to true to start expanded

    ImGui::SetNextItemOpen(isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::collapsingCheckboxHeader(component->getName().c_str(), &enabled, ImGuiTreeNodeFlags_None, onCheck))
    {
        EditorHelper::renderDynamicProperties(component, to_string(particleSystem->getTypeID()));
    }
}

void engine::ImGuiEditor::renderTerrainComponent(std::shared_ptr<TerrainComponent>& component)
{
    auto terrain = component->getTerrain();
    if (!terrain)
        return;

    bool enabled = component->isEnabled();

    std::function<void(bool)> onCheck = [component, &enabled](bool checked) {
        component->setEnabled(checked);
        enabled = component->isEnabled();
        };

    static bool isHeaderExpanded = true; // Set to true to start expanded

    ImGui::SetNextItemOpen(isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::collapsingCheckboxHeader(component->getName().c_str(), &enabled, ImGuiTreeNodeFlags_None, onCheck))
    {
        EditorHelper::renderDynamicProperties(component, to_string(terrain->getTypeID()));
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
    auto model = component->getModel();
    if (!model)
        return;

    bool enabled = component->isEnabled();

    std::function<void(bool)> onCheck = [component, &enabled](bool checked) {
        component->setEnabled(checked);
        enabled = component->isEnabled();
        };

    static bool isHeaderExpanded = true; // Set to true to start expanded

    ImGui::SetNextItemOpen(isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::collapsingCheckboxHeader(component->getName().c_str(), &enabled, ImGuiTreeNodeFlags_None, onCheck))
    {
        EditorHelper::renderDynamicProperties(component, to_string(model->getTypeID()));
    }
}

ImVec4 engine::ImGuiEditor::getEntityColor(const engine::EntityType entityType)
{
    auto color = engine::Colors::hexToNormalizedRGB("#969696");
    
    if (entityType == engine::EntityType::model) {
        color = engine::Colors::hexToNormalizedRGB("#d478ff");
    }
    else if (entityType == engine::EntityType::primitive) {
        color = engine::Colors::hexToNormalizedRGB("#abff78");
    }
    else if (entityType == engine::EntityType::light) {
        color = engine::Colors::hexToNormalizedRGB("#ffd83b");
    }
    else if (entityType == engine::EntityType::camera) {
        color = engine::Colors::hexToNormalizedRGB("#0f9cff");
    }
    else if (entityType == engine::EntityType::particleSystem) {
        color = engine::Colors::hexToNormalizedRGB("#ff9228");
    }
    else if (entityType == engine::EntityType::terrain) {
        color = engine::Colors::hexToNormalizedRGB("#ff28a7");
    }

    return ImVec4(color.r, color.g, color.b, color.a);
}

#endif

void engine::ImGuiEditor::renderGuizmo(const ImGuiID& dockspace_id, glm::mat4& projection, glm::mat4& view, const bool displayObjectTransformGuizmo)
{
    if (!m_guizmoCamera)
        return;

    // Convert glm::mat4 to const float*
    const float* projectionPtr = glm::value_ptr(projection);
    const float* viewPtr = glm::value_ptr(view);

    float* projectionPtr2 = glm::value_ptr(projection);
    float* viewPtr2 = glm::value_ptr(view);

    // Get the GLFW window position and size
    GLFWwindow* window = glfwGetCurrentContext();
    int windowX, windowY;
    glfwGetWindowPos(window, &windowX, &windowY);
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);


    ImGuizmo::BeginFrame();

    if (displayObjectTransformGuizmo)
    {
        ImGuizmo::SetOrthographic(!m_guizmoCamera->getIsPerspective());

        // Render the Editor window (no-decoration, for gizmo)
        ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);

        // Remove tab from dock panel
        ImGuiWindowClass window_class;
        window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
        ImGui::SetNextWindowClass(&window_class);

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

        ImGui::Begin("FloatingToolbar", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
       
        if (m_selectedEntity && m_selectedEntity->name != EntityManager::ROOT_ENTITY_NAME)
        {
            glm::mat4& objectMatrix = m_selectedEntity->getWorldTransform();
            float* objectMatrixPtr = glm::value_ptr(objectMatrix);

            for (int matId = 0; matId < gizmoCount; matId++)
            {
                ImGuizmo::SetID(matId);

                editTransform(viewPtr, projectionPtr2, glm::value_ptr(objectMatrix[matId]), lastUsing == matId, m_selectedEntity);
                if (ImGuizmo::IsUsing())
                {
                    lastUsing = matId;
                }
            }
        }

        ImGui::End();

        ImGui::PopStyleVar(1);
        ImGui::PopStyleColor(1);
    }
}

void engine::ImGuiEditor::editTransform(const float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition, std::shared_ptr<Entity> entity)
{
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
    static bool useSnap = false;
    static float snap[3] = { 1.f, 1.f, 1.f };
    static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
    static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
    static bool boundSizing = false;
    static bool boundSizingSnap = false;

    if (editTransformDecomposition)
    {
        EditorHelper::beginCenteredToolbar(3, 32);
        EditorHelper::addToolbarIconButton("translate", EditorIcon::editor_translate, []() { mCurrentGizmoOperation = ImGuizmo::TRANSLATE; });
        ImGui::SameLine();
        EditorHelper::addToolbarIconButton("rotate", EditorIcon::editor_rotate, []() { mCurrentGizmoOperation = ImGuizmo::ROTATE; });
        ImGui::SameLine();
        EditorHelper::addToolbarIconButton("scale", EditorIcon::editor_scale, []() { mCurrentGizmoOperation = ImGuizmo::SCALE; });
        EditorHelper::endCenteredToolbar();

        if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_T))
        {
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
            EditorHelper::resetIconToggleStates(); // Turn all off
            EditorHelper::setIconToggleState("translate", true); // Turn only this one on
        }
        else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_R))
        {
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
            EditorHelper::resetIconToggleStates(); // Turn all off
            EditorHelper::setIconToggleState("rotate", true); // Turn only this one on
        }
        else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_S))
        {
            mCurrentGizmoOperation = ImGuizmo::SCALE;
            EditorHelper::resetIconToggleStates(); // Turn all off
            EditorHelper::setIconToggleState("scale", true); // Turn only this one on
        }
    }


    //if (editTransformDecomposition)
    //{
    //    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_T))
    //        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    //    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_R))
    //        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    //    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_S)) // r Key
    //        mCurrentGizmoOperation = ImGuizmo::SCALE;
    //    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
    //        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    //    ImGui::SameLine();
    //    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
    //        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    //    ImGui::SameLine();
    //    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
    //        mCurrentGizmoOperation = ImGuizmo::SCALE;

    //    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    //    ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
    //    ImGui::InputFloat3("Tr", matrixTranslation);
    //    ImGui::InputFloat3("Rt", matrixRotation);
    //    ImGui::InputFloat3("Sc", matrixScale);
    //    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

    //    if (mCurrentGizmoOperation != ImGuizmo::SCALE)
    //    {
    //        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
    //            mCurrentGizmoMode = ImGuizmo::LOCAL;
    //        ImGui::SameLine();
    //        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
    //            mCurrentGizmoMode = ImGuizmo::WORLD;
    //    }
    //    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_F10))
    //        useSnap = !useSnap;
    //    ImGui::Checkbox("Snap", &useSnap);
    //    ImGui::SameLine();

    //    switch (mCurrentGizmoOperation)
    //    {
    //    case ImGuizmo::TRANSLATE:
    //        ImGui::InputFloat3("Snap", &snap[0]);
    //        break;
    //    case ImGuizmo::ROTATE:
    //        ImGui::InputFloat("Angle Snap", &snap[0]);
    //        break;
    //    case ImGuizmo::SCALE:
    //        ImGui::InputFloat("Scale Snap", &snap[0]);
    //        break;
    //    }
    //    ImGui::Checkbox("Bound Sizing", &boundSizing);
    //    if (boundSizing)
    //    {
    //        ImGui::PushID(3);
    //        ImGui::Checkbox("", &boundSizingSnap);
    //        ImGui::SameLine();
    //        ImGui::InputFloat3("Snap", boundsSnap);
    //        ImGui::PopID();
    //    }
    //}

    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    if (ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL))
    {
        float matrixTranslation2[3], matrixRotation2[3], matrixScale2[3];
        ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation2, matrixRotation2, matrixScale2);

        auto ttt = Transform{ glm::vec3(matrixTranslation2[0],matrixTranslation2[1], matrixTranslation2[2]), glm::vec3(matrixScale2[0], matrixScale2[1], matrixScale2[2]), glm::vec3(matrixRotation2[0], matrixRotation2[1], matrixRotation2[2]) };
        entity->setTransform(ttt);
        entity->updateSelfAndChild();
    }
}

void engine::ImGuiEditor::renderViewGuizmo(glm::mat4& projection, glm::mat4& view, bool displayViewTransformGuizmo)
{
    if (!m_guizmoCamera)
        return;

    if (displayViewTransformGuizmo)
    {
        ImGuiIO& io = ImGui::GetIO();

        // Calculate the guizmo position relative to the window's top-right corner
        ImVec2 pos = ImVec2(io.DisplaySize.x - 128.0f, 0.0f);
        ImVec2 size = ImVec2(128, 128);

        // MUTUALIZE !!!!!
        ImGuizmo::BeginFrame();

        // Convert glm::mat4 to const float*
        const float* projectionPtr = glm::value_ptr(projection);
        const float* viewPtr = glm::value_ptr(view);

        float* projectionPtr2 = glm::value_ptr(projection);
        float* viewPtr2 = glm::value_ptr(view);

        // box displayed in the upper right corner
        if (ImGuizmo::ViewManipulate(viewPtr2, camDistance, pos, size, 0x10101010))
        {
            // Get the updated view matrix
            glm::mat4 updatedViewMatrix = glm::make_mat4(viewPtr2);

            // Decompose the original view matrix to get its rotation and position
            glm::vec3 originalPosition, newPosition, scale;
            glm::quat originalRotation;

            // Decompose the original view matrix
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(view, scale, originalRotation, originalPosition, skew, perspective);

            // Decompose the updated view matrix to get the new position
            glm::decompose(updatedViewMatrix, scale, originalRotation, newPosition, skew, perspective);

            // Reconstruct the view matrix with the new position and the original rotation
            glm::mat4 newViewMatrix = glm::translate(glm::mat4(1.0f), newPosition) * glm::mat4_cast(originalRotation);

            // Set the new view matrix
            m_guizmoCamera->setFromViewMatrix(newViewMatrix);
        }
    }
}

void engine::ImGuiEditor::TestOverlay()
{
    ImGui::Begin("Overlay", nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav);

    ImGui::PushFont(ImGui::Spectrum::fontLarge);
    ImGui::TextColored(ImVec4(1, 0.2f, 0.2f, 1), "LARGE RED TITLE");
    ImGui::PopFont();

    ImGui::PushFont(ImGui::Spectrum::fontMedium);
    ImGui::TextColored(ImVec4(0.2f, 0.8f, 1, 1), "Medium cyan text");
    ImGui::PopFont();

    ImGui::PushFont(ImGui::Spectrum::fontSmall);
    ImGui::TextColored(ImVec4(1, 1, 0.2f, 1), "Small yellow text");
    ImGui::PopFont();

    ImGui::End();

    

}

