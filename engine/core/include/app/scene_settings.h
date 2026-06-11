#pragma once

#include <string>

#include "../common_defines.h"

#include "../lights/light.h"
#include "../misc/colors.h"

namespace engine
{
    enum class RenderMethod
    {
        Phong = 0,
        BlinnPhong = 1, // legacy
        PBR = 2, // mainstream (default)
    };

    struct GradientBackgroundSettings final
    {
        bool enabled{ false };
        Color topColor{ Colors::Black };
        Color bottomColor{ Colors::Black };
        float ySplit{ 0.5f };
    };

    // simple enum to send to shader
    enum PostProcessingEffect
    {
        None = 0,
        InvertColors = 1,
		Grayscale = 2,
		Sepia = 3,
		Blur = 4,
		EdgeDetection = 5,
		Vignette = 6,
		ChromaticAberration = 7,
		Pixelate = 8,
		Scanlines = 9,
		Bloom = 10
    };

    // TODO manage all other default values
    const RenderMethod DEFAULT_RENDER_METHOD{ RenderMethod::PBR };

    const float DEFAULT_SHADOW_MAPS_BIAS{ 0.001f };

    const float DEFAULT_PBR_IBL_DIFFUSE_INTENSITY{ 1.0f };
    const float DEFAULT_PBR_IBL_SPECULAR_INTENSITY{ 1.0f };

    const bool DEFAULT_SHOW_DEBUG_GRID{ false };

    const Color DEFAULT_BACKGROUND_COLOR{ Colors::Black };
    const GradientBackgroundSettings DEFAULT_BACKGROUND_GRADIENT_COLOR{ false, Colors::Black, Colors::Black, 0.0f };

    const bool DEFAULT_HIDE_PBR_HDR_SKYBOX{ false };
    const std::string DEFAULT_PBR_HDR_SKYBOX_FILEPATH{};
    const float DEFAULT_PBR_HDR_SKYBOX_BLUR{ 0.0f };


    const bool DEFAULT_ENABLE_SHADOWS{ true };
    const ShadowCalculationMethod DEFAULT_SHADOWS_METHOD{ ShadowCalculationMethod::PCFSoft };
    const float DEFAULT_SHADOWS_INTENSITY{ 1.5f };
    const float DEFAULT_SHADOWMAP_TEXTURE_SIZE{ 2048.0f };
    const float DEFAULT_SHADOWS_BLUR{ 1.0f };


    const float DEFAULT_EXPOSURE{ 1.0f };
    const bool DEFAULT_ENABLE_GAMMA_CORRECTION{ false };
    const bool DEFAULT_ENABLE_TONE_MAPPING{ false };
    const PostProcessingEffect DEFAULT_POST_PROCESSING_FX{ PostProcessingEffect::None };

    struct SceneSettings final
    {
        RenderMethod method{ DEFAULT_RENDER_METHOD };

		Color backgroundColor{ DEFAULT_BACKGROUND_COLOR };
        GradientBackgroundSettings backgroundGradientColors { DEFAULT_BACKGROUND_GRADIENT_COLOR };

        bool showDebugGrid{ DEFAULT_SHOW_DEBUG_GRID };

        bool HDRSkyboxHide{ DEFAULT_HIDE_PBR_HDR_SKYBOX };
        std::string HDRSkyboxFilePath{ DEFAULT_PBR_HDR_SKYBOX_FILEPATH };
        float HDRSkyboxBlurStrength{ DEFAULT_PBR_HDR_SKYBOX_BLUR };

        bool enableShadows{ DEFAULT_ENABLE_SHADOWS };
        ShadowCalculationMethod shadowCalculationMethod{ DEFAULT_SHADOWS_METHOD };
        float shadowIntensity{ DEFAULT_SHADOWS_INTENSITY };
        float shadowMapsTextureSize{ DEFAULT_SHADOWMAP_TEXTURE_SIZE };
        float shadowMapsBiasFactor{ DEFAULT_SHADOW_MAPS_BIAS };
        float shadowMapsBlur{ DEFAULT_SHADOWS_BLUR };
        
        float iblDiffuseIntensity{ DEFAULT_PBR_IBL_DIFFUSE_INTENSITY };
        float iblSpecularIntensity{ DEFAULT_PBR_IBL_SPECULAR_INTENSITY };

        float exposure{ DEFAULT_EXPOSURE };
        bool enableGammaCorrection{ DEFAULT_ENABLE_GAMMA_CORRECTION };
        bool enableToneMapping{ DEFAULT_ENABLE_TONE_MAPPING };
        PostProcessingEffect applyPostProcessFx{ DEFAULT_POST_PROCESSING_FX };
        
        // to finish
        bool enableFaceCulling{ true };
        bool enableCameraFrustrumCulling{ true };
        bool drawAsWireframe{ false };

        bool drawLightsVisualHelpers{ false };
        bool drawBoundingBoxesVisualHelpers{ false };
        bool drawNormalsVisualHelpers{ false };
    };
}