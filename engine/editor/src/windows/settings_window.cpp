#include "../../include/windows/settings_window.h"

#include "../../../core/include/singleton.h"

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

    sceneSetting_enableBloom = settings.enableBloom;
    sceneSetting_bloomThreshold = settings.bloomThreshold;
    sceneSetting_bloomStrength = settings.bloomStrength;
    sceneSetting_bloomBlurPasses = settings.bloomBlurPasses;
}

void engine::SettingsWindow::renderTabSettings()
{
    ImGui::BeginChild("SettingsRegion", ImVec2(0, 0), true);

    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
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
    if (EditorHelper::renderToggleButtonWithLabel("Wireframe", sceneSetting_drawAsWireframe, lastDrawWireframe))
    {
        emit(UIEventType::SceneSettingChanged, "draw_wireframe", sceneSetting_drawAsWireframe);
    }

    static ubyte lastFrameBufferMsaaSamples = DEFAULT_FRAMEBUFFER_MSAA_SAMPLES;
    if (EditorHelper::renderSliderUnsignedByteWithLabel("Framebuffer antialiasing samples", sceneSetting_framebufferMsaaSamples, lastFrameBufferMsaaSamples, 0, 16))
    {
        emit(UIEventType::SceneSettingChanged, "framebuffer_msaa_samples", sceneSetting_framebufferMsaaSamples);
    }

    static bool lastEnableGammaCorection = DEFAULT_ENABLE_GAMMA_CORRECTION;
    if (EditorHelper::renderToggleButtonWithLabel("Gamma correction", sceneSetting_enableGammaCorrection, lastEnableGammaCorection))
    {
        emit(UIEventType::SceneSettingChanged, "enable_gamma_correction", sceneSetting_enableGammaCorrection);
    }

    static bool lastEnableToneMapping = DEFAULT_ENABLE_TONE_MAPPING;
    if (EditorHelper::renderToggleButtonWithLabel("Tone mapping", sceneSetting_enableToneMapping, lastEnableToneMapping))
    {
        emit(UIEventType::SceneSettingChanged, "enable_tone_mapping", sceneSetting_enableToneMapping);
    }

    static float lastExposure = DEFAULT_EXPOSURE;
    if (EditorHelper::renderDragFloatWithLabel("Exposure", sceneSetting_exposure, lastExposure, 0.0f, 5.0f, 0.1f))
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
    if (EditorHelper::renderToggleButtonWithLabel("Face culling", sceneSetting_enableFaceCulling, lastEnableFaceCulling))
    {
        emit(UIEventType::SceneSettingChanged, "enable_face_culling", sceneSetting_enableFaceCulling);
    }

    static bool lastEnableCameraFrustrumCulling = DEFAULT_ENABLE_CAMERA_FRUSTRUM_CULLING;
    if (EditorHelper::renderToggleButtonWithLabel("Camera frustrum culling", sceneSetting_enableCameraFrustrumCulling, lastEnableCameraFrustrumCulling))
    {
        emit(UIEventType::SceneSettingChanged, "enable_camera_frustrum_culling", sceneSetting_enableCameraFrustrumCulling);
    }

    // ------------------------------------------------------------------------
    // Visual helpers
    // ------------------------------------------------------------------------

    bool isEnableVisualHelpersHeaderExpanded = true;


    ImGui::Dummy(ImVec2(0, 8));

    ImGui::SetNextItemOpen(isEnableVisualHelpersHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::renderHeaderSeparator("Visual helpers", sceneSetting_enableBloom, EditorHelper::im_grey_dark))
    {
        static bool lastDrawLightsVisualHelpers = DEFAULT_ENABLE_LIGHTS_VISUAL_HELPERS;
        if (EditorHelper::renderToggleButtonWithLabel("Lights helpers", sceneSetting_drawLightsVisualHelpers, lastDrawLightsVisualHelpers, 15.0f))
        {
            emit(UIEventType::SceneSettingChanged, "draw_lights_visual_helpers", sceneSetting_drawLightsVisualHelpers);
        }

        static bool lastDrawBoundingBoxesVisualHelpers = DEFAULT_ENABLE_BOUNDINGBOX_VISUAL_HELPERS;
        if (EditorHelper::renderToggleButtonWithLabel("Bounding boxes", sceneSetting_drawBoundingBoxesVisualHelpers, lastDrawBoundingBoxesVisualHelpers, 15.0f))
        {
            emit(UIEventType::SceneSettingChanged, "draw_bounding_boxes_visual_helpers", sceneSetting_drawBoundingBoxesVisualHelpers);
        }

        static bool lastDrawDebugNormalsVisualHelpers = DEFAULT_ENABLE_NORMALS_VISUAL_HELPERS;
        if (EditorHelper::renderToggleButtonWithLabel("Normals helpers", sceneSetting_drawDebugNormalsVisualHelpers, lastDrawDebugNormalsVisualHelpers, 15.0f))
        {
            emit(UIEventType::SceneSettingChanged, "draw_debug_normals_visual_helpers", sceneSetting_drawDebugNormalsVisualHelpers);
        }
    }


    

    // ------------------------------------------------------------------------
    // shadows
    // ------------------------------------------------------------------------

    bool isEnableShadowsHeaderExpanded = sceneSetting_enableShadows;

    std::function<void(bool)> onCheckEnableShadowsHeader = [this, &isEnableShadowsHeaderExpanded](bool checked) {
        isEnableShadowsHeaderExpanded = checked;
        emit(UIEventType::SceneSettingChanged, "enable_shadows", sceneSetting_enableShadows);
        };

    ImGui::Dummy(ImVec2(0, 8));

    ImGui::SetNextItemOpen(isEnableShadowsHeaderExpanded, ImGuiCond_Always);
    if (EditorHelper::renderToggleHeaderSeparator("Enable shadows maps", sceneSetting_enableShadows, EditorHelper::im_grey_dark, onCheckEnableShadowsHeader))
    {
        static int lastShadowCalculationMethod = static_cast<int>(DEFAULT_SHADOWS_METHOD);
        std::vector<std::string> items{ "Default", "PCF", "PCFSoft", "PCSS" };
        std::function<void(unsigned short)> callback = [this](unsigned short value) {
            sceneSetting_shadowCalculationMethod = value; // starting at 0 because of enum default
            emit(UIEventType::SceneSettingChanged, "shadow_calculation_method", sceneSetting_shadowCalculationMethod);
            };
        EditorHelper::renderComboBoxWithLabel("Method", sceneSetting_shadowCalculationMethod, items, callback, 15.0f);


        static int lastShadowMapTextureSize = static_cast<int>(DEFAULT_SHADOWMAP_TEXTURE_SIZE);
        if (EditorHelper::renderSliderIntWithLabel("Texture size", sceneSetting_shadowMapTextureSize, lastShadowMapTextureSize, 256, 4096, 15.0f))
        {
            emit(UIEventType::SceneSettingChanged, "shadow_maps_texture_size", sceneSetting_shadowMapTextureSize);
        }

        static float lastShadowIntensity = DEFAULT_SHADOWS_INTENSITY;
        if (EditorHelper::renderDragFloatWithLabel("Intensity", sceneSetting_shadowIntensity, lastShadowIntensity, 0.0f, 5.0f, 0.1f, "%.1f", 15.0f))
        {
            emit(UIEventType::SceneSettingChanged, "shadow_intensity", sceneSetting_shadowIntensity);
        }

        // Typical values(tune slightly per scene)
        // material.shadowMapsBias = 0.02;   // indoor
        // material.shadowMapsBias = 0.05;   // outdoor / large scenes
        static float lastShadowMapsBiasFactor = DEFAULT_SHADOW_MAPS_BIAS;
        if (EditorHelper::renderDragFloatWithLabel("Bias", sceneSetting_shadowMapBiasFactor, lastShadowMapsBiasFactor, 0.001f, 0.05f, 0.001f, "%.3f", 15.0f))
        {
            emit(UIEventType::SceneSettingChanged, "shadow_maps_bias_factor", sceneSetting_shadowMapBiasFactor);
        }

        static float lastShadowMapsBlur = DEFAULT_SHADOWS_BLUR;
        if (EditorHelper::renderDragFloatWithLabel("Blur", sceneSetting_shadowMapBlur, lastShadowMapsBlur, 0.0f, 50.0f, 0.1f, "%.1f", 15.0f))
        {
            emit(UIEventType::SceneSettingChanged, "shadow_maps_blur_factor", sceneSetting_shadowMapBlur);
        }

        ImGui::Dummy(ImVec2(0, 10));
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


    // ------------------------------------------------------------------------
    // bloom
    // ------------------------------------------------------------------------

    bool isEnableBloomHeaderExpanded = sceneSetting_enableBloom;

    std::function<void(bool)> onCheckEnableBloomHeader = [this, &isEnableBloomHeaderExpanded](bool checked) {
        isEnableBloomHeaderExpanded = checked;
        emit(UIEventType::SceneSettingChanged, "enable_bloom", sceneSetting_enableBloom);
        };

    ImGui::Dummy(ImVec2(0, 8));

    ImGui::SetNextItemOpen(isEnableBloomHeaderExpanded, ImGuiCond_Always);
    if (EditorHelper::renderToggleHeaderSeparator("Enable bloom", sceneSetting_enableBloom, EditorHelper::im_grey_dark, onCheckEnableBloomHeader))
    {
        static float lastBloomThreshold = DEFAULT_BLOOM_THRESHOLD;
        if (EditorHelper::renderDragFloatWithLabel("Bloom threshold", sceneSetting_bloomThreshold, lastBloomThreshold, 0.0f, 10.0f, 0.1f, "%.3f", 15.0f))
        {
            emit(UIEventType::SceneSettingChanged, "bloom_threshold", sceneSetting_bloomThreshold);
        }

        static float lastBloomStrength = DEFAULT_BLOOM_STRENGTH;
        if (EditorHelper::renderDragFloatWithLabel("Bloom strength", sceneSetting_bloomStrength, lastBloomStrength, 0.0f, 10.0f, 0.1f, "%.3f", 15.0f))
        {
            emit(UIEventType::SceneSettingChanged, "bloom_strength", sceneSetting_bloomStrength);
        }

        static ubyte lastBloomBlurPasses = DEFAULT_BLOOM_BLUR_PASSES;
        if (EditorHelper::renderDragUnsignedByteWithLabel("Bloom blur passes", sceneSetting_bloomBlurPasses, lastBloomBlurPasses, 0, 10, 1, 15.0f))
        {
            emit(UIEventType::SceneSettingChanged, "bloom_blur_passes", sceneSetting_bloomBlurPasses);
        }
    }



    

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    ImGui::EndChild();
}