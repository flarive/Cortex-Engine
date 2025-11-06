#pragma once

#include <string>

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

        float shadowIntensity{ 1.5f };
        float iblDiffuseIntensity{ 1.0f };
        float iblSpecularIntensity{ 1.0f };

        bool enableGammaCorrection{ false };
        bool enableFaceCulling{ true };
        bool enableCameraFrustrumCulling{ true };
        bool drawAsWireframe{ false };

        bool drawLightsVisualHelpers{ false };
        bool drawBoundingBoxesVisualHelpers{ false };
        bool drawNormalsVisualHelpers{ false };

        bool enableShadows{ true };
        GLsizei shadowMapsTextureSize = 2048;
    };
}