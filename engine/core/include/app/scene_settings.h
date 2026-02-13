#pragma once

#include <string>

#include "../common_defines.h"

#include "../lights/light.h"
#include "../misc/colors.h"

namespace engine
{
    enum class RenderMethod
    {
        BlinnPhong = 0, // legacy
        PBR = 1 // mainstream
    };

    struct GradientBackgroundSettings final
    {
        bool enabled{ false };
        Color topColor{ Colors::Black };
        Color bottomColor{ Colors::Black };
        float ySplit{ 0.5f };
    };
    

    struct SceneSettings final
    {
        RenderMethod method{};

		Color backgroundColor{ Colors::Black };
        GradientBackgroundSettings backgroundGradientColors { false, Colors::Black, Colors::Black, 0.0f};

        bool showDebugGrid{ false };

        bool HDRSkyboxHide{ false };
        std::string HDRSkyboxFilePath{};
        float HDRSkyboxBlurStrength{ 0.0f };

        bool enableShadows{ true };
        int shadowCalculationMethod{ static_cast<int>(ShadowCalculationMethod::PCFSoft) };
        float shadowIntensity{ 1.5f };
        float shadowMapsTextureSize{ 2048.0f };
        float shadowMapsBiasFactor{ 0.001f };
        float shadowMapsBlur{ 1.0f };
        
        float iblDiffuseIntensity{ 1.0f };
        float iblSpecularIntensity{ 1.0f };

        bool enableGammaCorrection{ false };
        bool enableFaceCulling{ true };
        bool enableCameraFrustrumCulling{ true };
        bool drawAsWireframe{ false };

        bool drawLightsVisualHelpers{ false };
        bool drawBoundingBoxesVisualHelpers{ false };
        bool drawNormalsVisualHelpers{ false };
    };

    
}