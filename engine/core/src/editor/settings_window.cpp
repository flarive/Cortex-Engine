#include "../../include/editor/settings_window.h"




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
    EditorHelper::renderSliderIntWithLabel("Render method", "render_method", sceneSetting_renderMethod, lastRenderMethod, 1, 2, m_onSceneSettingChanged);

    static bool lastDrawWireframe = DEFAULT_ENABLE_WIREFRAME_MODE;
    if (ImGui::Toggle("Wireframe", &sceneSetting_drawAsWireframe, toggle_config))
    {
        if (m_onSceneSettingChanged && lastDrawWireframe != sceneSetting_drawAsWireframe)
        {
            emit(UIEventType::SceneSettingChanged, "draw_wireframe", sceneSetting_drawAsWireframe);
            //m_onSceneSettingChanged("draw_wireframe", sceneSetting_drawAsWireframe);
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