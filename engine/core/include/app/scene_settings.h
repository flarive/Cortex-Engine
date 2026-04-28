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

    const float DEFAULT_SHADOW_MAPS_BIAS = 0.001f;

    struct SceneSettings final
    {
        RenderMethod method{ RenderMethod ::PBR };

		Color backgroundColor{ Colors::Black };
        GradientBackgroundSettings backgroundGradientColors { false, Colors::Black, Colors::Black, 0.0f};

        bool showDebugGrid{ false };

        bool HDRSkyboxHide{ false };
        std::string HDRSkyboxFilePath{};
        float HDRSkyboxBlurStrength{ 0.0f };

        bool enableShadows{ true };
        ShadowCalculationMethod shadowCalculationMethod{ ShadowCalculationMethod::PCFSoft };
        float shadowIntensity{ 1.5f };
        float shadowMapsTextureSize{ 2048.0f };
        float shadowMapsBiasFactor{ DEFAULT_SHADOW_MAPS_BIAS };
        float shadowMapsBlur{ 1.0f };
        
        float iblDiffuseIntensity{ 1.0f };
        float iblSpecularIntensity{ 1.0f };

        float exposure{ 1.0f };
        bool enableGammaCorrection{ false };
        bool enableToneMapping{ false };
        PostProcessingEffect applyPostProcessFx{ PostProcessingEffect::None };
        bool enableFaceCulling{ true };
        bool enableCameraFrustrumCulling{ true };
        bool drawAsWireframe{ false };

        bool drawLightsVisualHelpers{ false };
        bool drawBoundingBoxesVisualHelpers{ false };
        bool drawNormalsVisualHelpers{ false };
    };
}