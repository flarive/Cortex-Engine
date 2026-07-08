#pragma once

#include "imgui_element.h"

#include "../app/scene_settings.h"

#include "../../include/editor/editor_helper.h"

namespace engine
{
    class SettingsWindow : public ImGuiElement
    {
    public:
        SettingsWindow() : ImGuiElement(Category::Window, "Settings") {}

    private:
		std::function<void(std::string, SceneSetting)> m_onSceneSettingChanged; // << callback

		void renderTabSettings();

    protected:
        
		int sceneSetting_renderMethod{ static_cast<int>(DEFAULT_RENDER_METHOD) };
		bool sceneSetting_drawAsWireframe{ DEFAULT_ENABLE_WIREFRAME_MODE };
		float sceneSetting_exposure{ DEFAULT_EXPOSURE };
		bool sceneSetting_enableGammaCorrection{ DEFAULT_ENABLE_GAMMA_CORRECTION };
		bool sceneSetting_enableToneMapping{ DEFAULT_ENABLE_TONE_MAPPING };
		int sceneSetting_applyPostProcessFx{ static_cast<int>(DEFAULT_POST_PROCESSING_FX) };
		bool sceneSetting_enableFaceCulling{ DEFAULT_ENABLE_FACE_CULLING };
		bool sceneSetting_enableCameraFrustrumCulling{ DEFAULT_ENABLE_CAMERA_FRUSTRUM_CULLING };
		bool sceneSetting_drawLightsVisualHelpers{ DEFAULT_ENABLE_LIGHTS_VISUAL_HELPERS };
		bool sceneSetting_drawBoundingBoxesVisualHelpers{ DEFAULT_ENABLE_BOUNDINGBOX_VISUAL_HELPERS };
		bool sceneSetting_drawDebugNormalsVisualHelpers{ DEFAULT_ENABLE_NORMALS_VISUAL_HELPERS };
		bool sceneSetting_enableShadows{ DEFAULT_ENABLE_SHADOWS };
		int sceneSetting_shadowCalculationMethod{ static_cast<int>(DEFAULT_SHADOWS_METHOD) };
		float sceneSetting_shadowIntensity{ DEFAULT_SHADOWS_INTENSITY };
		int sceneSetting_shadowMapTextureSize{ static_cast<int>(DEFAULT_SHADOWMAP_TEXTURE_SIZE) };
		float sceneSetting_shadowMapBiasFactor{ DEFAULT_SHADOW_MAPS_BIAS };
		float sceneSetting_shadowMapBlur{ DEFAULT_SHADOWS_BLUR };

		float sceneSetting_iblDiffuseIntensity{ DEFAULT_PBR_IBL_DIFFUSE_INTENSITY };
		float sceneSetting_iblSpecularIntensity{ DEFAULT_PBR_IBL_SPECULAR_INTENSITY };

		ubyte sceneSetting_framebufferMsaaSamples{ DEFAULT_FRAMEBUFFER_MSAA_SAMPLES };

        void draw() override
        {
            renderTabSettings();
        }
    };
}