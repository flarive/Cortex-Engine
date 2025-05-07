#pragma once

#include <string>

namespace engine
{
    enum class RenderMethod
    {
        BlinnPhong = 0, // legacy
        PBR = 1 // mainstream
    };

    struct SceneSettings
    {
        RenderMethod method{};

        bool HDRSkyboxHide{ false };
        std::string HDRSkyboxFilePath{};
        float HDRSkyboxBlurStrength{ 0.0f };

        float shadowIntensity{ 1.5f };
        float iblDiffuseIntensity{ 1.0f };
        float iblSpecularIntensity{ 1.0f };

        bool applyGammaCorrection{};
    };
}