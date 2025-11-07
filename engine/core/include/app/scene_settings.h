#pragma once

#include <string>

#include "../common_defines.h"

namespace engine
{
    enum class RenderMethod
    {
        BlinnPhong = 0, // legacy
        PBR = 1 // mainstream
    };

    struct SceneSettings final
    {
        RenderMethod method{};

        bool HDRSkyboxHide{ false };
        std::string HDRSkyboxFilePath{};
        float HDRSkyboxBlurStrength{ 0.0f };

        bool enableShadows{ true };
        float shadowIntensity{ 1.5f };
        float shadowMapsTextureSize{ 2048.0f };
        float shadowMapsBiasFactor{ 0.001f };
        
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