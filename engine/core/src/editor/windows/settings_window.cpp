#include "../../../include/editor/windows/settings_window.h"

#include "../../../include/singleton.h"

void engine::SettingsWindow::init()
{
    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    const SceneSettings& settings = singleton->sceneSettings();

    sceneSetting_renderMethod = static_cast<int>(settings.method);
    sceneSetting_drawAsWireframe = settings.drawAsWireframe;
    sceneSetting_exposure = settings.exposure;
    sceneSetting_enableGammaCorrection = settings.enableGammaCorrection;
    sceneSetting_enableToneMapping = settings.enableToneMapping;
    sceneSetting_applyPostProcessFx = static_cast<int>(settings.applyPostProcessFx);
	sceneSetting_enableFaceCulling = settings.enableFaceCulling;
	sceneSetting_enableCameraFrustrumCulling = settings.enableCameraFrustrumCulling;
	sceneSetting_drawLightsVisualHelpers = settings.drawLightsVisualHelpers;
	sceneSetting_drawBoundingBoxesVisualHelpers = settings.drawBoundingBoxesVisualHelpers;
	sceneSetting_drawDebugNormalsVisualHelpers = settings.drawNormalsVisualHelpers;
	sceneSetting_enableShadows = settings.enableShadows;
    sceneSetting_shadowCalculationMethod = static_cast<int>(settings.shadowCalculationMethod);
	sceneSetting_shadowIntensity = settings.shadowIntensity;
    sceneSetting_shadowMapTextureSize = static_cast<int>(settings.shadowMapsTextureSize);
	sceneSetting_shadowMapBiasFactor = settings.shadowMapsBiasFactor;
	sceneSetting_shadowMapBlur = settings.shadowMapsBlur;
    sceneSetting_iblDiffuseIntensity = settings.iblDiffuseIntensity;
    sceneSetting_iblSpecularIntensity = settings.iblSpecularIntensity;
	sceneSetting_framebufferMsaaSamples = settings.frameBufferAntiAliasingSamplesQuality;
}

void engine::SettingsWindow::renderTabSettings()
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
    if (EditorHelper::renderSliderIntWithLabel("Render method", sceneSetting_renderMethod, lastRenderMethod, 1, 2))
    {
        emit(UIEventType::SceneSettingChanged, "render_method", sceneSetting_renderMethod);
    }

    static bool lastDrawWireframe = DEFAULT_ENABLE_WIREFRAME_MODE;
    if (ImGui::Toggle("Wireframe", &sceneSetting_drawAsWireframe, toggle_config))
    {
        if (lastDrawWireframe != sceneSetting_drawAsWireframe)
        {
            emit(UIEventType::SceneSettingChanged, "draw_wireframe", sceneSetting_drawAsWireframe);
            lastDrawWireframe = sceneSetting_drawAsWireframe;
        }
    }

    static ubyte lastFrameBufferMsaaSamples = DEFAULT_FRAMEBUFFER_MSAA_SAMPLES;
    if (EditorHelper::renderSliderUnsignedByteWithLabel("Framebuffer antialiasing samples", sceneSetting_framebufferMsaaSamples, lastFrameBufferMsaaSamples, 0, 16))
    {
        emit(UIEventType::SceneSettingChanged, "framebuffer_msaa_samples", sceneSetting_framebufferMsaaSamples);
    }

    static bool lastEnableGammaCorection = DEFAULT_ENABLE_GAMMA_CORRECTION;
    if (ImGui::Toggle("Gamma correction", &sceneSetting_enableGammaCorrection, toggle_config))
    {
        if (lastEnableGammaCorection != sceneSetting_enableGammaCorrection)
        {
            emit(UIEventType::SceneSettingChanged, "enable_gamma_correction", sceneSetting_enableGammaCorrection);
            lastEnableGammaCorection = sceneSetting_enableGammaCorrection;
        }
    }

    static bool lastEnableToneMapping = DEFAULT_ENABLE_TONE_MAPPING;
    if (ImGui::Toggle("Tone mapping", &sceneSetting_enableToneMapping, toggle_config))
    {
        if (lastEnableToneMapping != sceneSetting_enableToneMapping)
        {
            emit(UIEventType::SceneSettingChanged, "enable_tone_mapping", sceneSetting_enableToneMapping);
            lastEnableToneMapping = sceneSetting_enableToneMapping;
        }
    }

    static float lastExposure = DEFAULT_EXPOSURE;
    if (EditorHelper::renderDragFloatWithLabel("Exposure", sceneSetting_exposure, lastExposure, 0.0f, 5.0f, 0.1f, "%.1f"))
    {
        emit(UIEventType::SceneSettingChanged, "exposure", sceneSetting_exposure);
    }


    static int lastApplyPostProcessFx = DEFAULT_POST_PROCESSING_FX;
    if (EditorHelper::renderSliderIntWithLabel("Post process", sceneSetting_applyPostProcessFx, lastApplyPostProcessFx, 0, 10))
    {
        emit(UIEventType::SceneSettingChanged, "post_process", sceneSetting_applyPostProcessFx);
    }

    // ------------------------------------------------------------------------
    // optims
    // ------------------------------------------------------------------------

    static bool lastEnableFaceCulling = DEFAULT_ENABLE_FACE_CULLING;
    if (ImGui::Toggle("Face culling", &sceneSetting_enableFaceCulling, toggle_config))
    {
        if (lastEnableFaceCulling != sceneSetting_enableFaceCulling)
        {
            emit(UIEventType::SceneSettingChanged, "enable_face_culling", sceneSetting_enableFaceCulling);
            lastEnableFaceCulling = sceneSetting_enableFaceCulling;
        }
    }

    static bool lastEnableCameraFrustrumCulling = DEFAULT_ENABLE_CAMERA_FRUSTRUM_CULLING;
    if (ImGui::Toggle("Camera frustrum culling", &sceneSetting_enableCameraFrustrumCulling, toggle_config))
    {
        if (lastEnableCameraFrustrumCulling != sceneSetting_enableCameraFrustrumCulling)
        {
            emit(UIEventType::SceneSettingChanged, "enable_camera_frustrum_culling", sceneSetting_enableCameraFrustrumCulling);
            lastEnableCameraFrustrumCulling = sceneSetting_enableCameraFrustrumCulling;
        }
    }

    // ------------------------------------------------------------------------
    // Visual helpers
    // ------------------------------------------------------------------------

    static bool lastDrawLightsVisualHelpers = DEFAULT_ENABLE_LIGHTS_VISUAL_HELPERS;
    if (ImGui::Toggle("Lights visual helpers", &sceneSetting_drawLightsVisualHelpers, toggle_config))
    {
        if (lastDrawLightsVisualHelpers != sceneSetting_drawLightsVisualHelpers)
        {
            emit(UIEventType::SceneSettingChanged, "draw_lights_visual_helpers", sceneSetting_drawLightsVisualHelpers);
            lastDrawLightsVisualHelpers = sceneSetting_drawLightsVisualHelpers;
        }
    }

    static bool lastDrawBoundingBoxesVisualHelpers = DEFAULT_ENABLE_BOUNDINGBOX_VISUAL_HELPERS;
    if (ImGui::Toggle("Bounding boxes visual helpers", &sceneSetting_drawBoundingBoxesVisualHelpers, toggle_config))
    {
        if (lastDrawBoundingBoxesVisualHelpers != sceneSetting_drawBoundingBoxesVisualHelpers)
        {
            emit(UIEventType::SceneSettingChanged, "draw_bounding_boxes_visual_helpers", sceneSetting_drawBoundingBoxesVisualHelpers);
            lastDrawBoundingBoxesVisualHelpers = sceneSetting_drawBoundingBoxesVisualHelpers;
        }
    }

    static bool lastDrawDebugNormalsVisualHelpers = DEFAULT_ENABLE_NORMALS_VISUAL_HELPERS;
    if (ImGui::Toggle("Normals visual helpers", &sceneSetting_drawDebugNormalsVisualHelpers, toggle_config))
    {
        if (lastDrawDebugNormalsVisualHelpers != sceneSetting_drawDebugNormalsVisualHelpers)
        {
            emit(UIEventType::SceneSettingChanged, "draw_debug_normals_visual_helpers", sceneSetting_drawDebugNormalsVisualHelpers);
            lastDrawDebugNormalsVisualHelpers = sceneSetting_drawDebugNormalsVisualHelpers;
        }
    }

    // ------------------------------------------------------------------------
    // shadows
    // ------------------------------------------------------------------------

    static bool lastEnableShadows = DEFAULT_ENABLE_SHADOWS;
    if (ImGui::Toggle("Enable shadows", &sceneSetting_enableShadows, toggle_config))
    {
        if (lastEnableShadows != sceneSetting_enableShadows)
        {
            emit(UIEventType::SceneSettingChanged, "enable_shadows", sceneSetting_enableShadows);
            lastEnableShadows = sceneSetting_enableShadows;
        }
    }

    static int lastShadowCalculationMethod = static_cast<int>(DEFAULT_SHADOWS_METHOD);
    if (EditorHelper::renderSliderIntWithLabel("Shadow maps method", sceneSetting_shadowCalculationMethod, lastShadowCalculationMethod, 1, 3))
    {
        emit(UIEventType::SceneSettingChanged, "shadow_calculation_method", sceneSetting_shadowCalculationMethod);
    }

    static int lastShadowMapTextureSize = static_cast<int>(DEFAULT_SHADOWMAP_TEXTURE_SIZE);
    if (EditorHelper::renderSliderIntWithLabel("Shadow maps texture size", sceneSetting_shadowMapTextureSize, lastShadowMapTextureSize, 256, 4096))
    {
        emit(UIEventType::SceneSettingChanged, "shadow_maps_texture_size", sceneSetting_shadowMapTextureSize);
    }

    static float lastShadowIntensity = DEFAULT_SHADOWS_INTENSITY;
    if (EditorHelper::renderDragFloatWithLabel("Shadow maps intensity", sceneSetting_shadowIntensity, lastShadowIntensity, 0.0f, 5.0f, 0.1f, "%.1f"))
    {
        emit(UIEventType::SceneSettingChanged, "shadow_intensity", sceneSetting_shadowIntensity);
    }

    // Typical values(tune slightly per scene)
    // material.shadowMapsBias = 0.02;   // indoor
    // material.shadowMapsBias = 0.05;   // outdoor / large scenes
    static float lastShadowMapsBiasFactor = DEFAULT_SHADOW_MAPS_BIAS;
    if (EditorHelper::renderDragFloatWithLabel("Shadow maps bias", sceneSetting_shadowMapBiasFactor, lastShadowMapsBiasFactor, 0.001f, 0.05f, 0.001f, "%.3f"))
    {
        emit(UIEventType::SceneSettingChanged, "shadow_maps_bias_factor", sceneSetting_shadowMapBiasFactor);
    }

    static float lastShadowMapsBlur = DEFAULT_SHADOWS_BLUR;
    if (EditorHelper::renderDragFloatWithLabel("Shadow maps blur", sceneSetting_shadowMapBlur, lastShadowMapsBlur, 0.0f, 50.0f, 0.1f, "%.1f"))
    {
        emit(UIEventType::SceneSettingChanged, "shadow_maps_blur_factor", sceneSetting_shadowMapBlur);
    }


    // ------------------------------------------------------------------------
    // PBR renderer only !!!!
    // ------------------------------------------------------------------------

    if (sceneSetting_renderMethod == static_cast<int>(RenderMethod::PBR))
    {
        static float lastIblDiffuseIntensity = DEFAULT_PBR_IBL_DIFFUSE_INTENSITY;
        if (EditorHelper::renderDragFloatWithLabel("IBL Diffuse Intensity", sceneSetting_iblDiffuseIntensity, lastIblDiffuseIntensity, 0.0f, 10.0f, 0.1f, "%.1f"))
        {
            emit(UIEventType::SceneSettingChanged, "pbr_ibl_diffuse_intensity", sceneSetting_iblDiffuseIntensity);
        }

        static float lastIblSpecularIntensity = DEFAULT_PBR_IBL_SPECULAR_INTENSITY;
        if (EditorHelper::renderDragFloatWithLabel("IBL Specular Intensity", sceneSetting_iblSpecularIntensity, lastIblSpecularIntensity, 0.0f, 10.0f, 0.1f, "%.1f"))
        {
            emit(UIEventType::SceneSettingChanged, "pbr_ibl_specular_intensity", sceneSetting_iblSpecularIntensity);
        }
    }

    ImGui::PopStyleVar();

    ImGui::PopStyleColor();

    ImGui::EndChild();
}