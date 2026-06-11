#pragma once

#include "../tools/system_monitor.h"

#include "../ecs/entity.h"
#include "../ecs/transform_component.h"
#include "../ecs/light_component.h"
#include "../ecs/camera_component.h"
#include "../ecs/model_component.h"
#include "../ecs/primitive_component.h"
#include "../ecs/animator_component.h"
#include "../ecs/particlesystem_component.h"
#include "../ecs/terrain_component.h"

#include "../app/scene_settings.h"

#include "../lights/light.h"

#include "editor_helper.h"

#include <variant>

#include <imgui.h>

#if EDITOR_MODE
namespace engine
{
	class ImGuiEditor final
	{
	public:
		ImGuiEditor() = default;
		~ImGuiEditor() = default;

		void setScene(std::shared_ptr<Entity> rootEntity);

		void renderUIWindow(bool show, glm::mat4& projection, glm::mat4& view, const bool displayObjectTransformGuizmo);

		// Let parent register a callback
		void setOnSelectionChanged(std::function<void(std::shared_ptr<Entity>)> callback) {
			m_onSelectionChanged = std::move(callback);
		}

		// Let parent register a callback
		void setOnSceneSettingChanged(std::function<void(const std::string&, SceneSetting)> callback) {
			m_onSceneSettingChanged = std::move(callback);
		}


		void initRenderGuizmo(const std::shared_ptr<Camera> camera);

		

		void renderGuizmo(const ImGuiID& dockspace_id, glm::mat4& projection, glm::mat4& view, const bool displayObjectTransformGuizmo);

		void editTransform(const float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition, std::shared_ptr<Entity> entity);

		void renderViewGuizmo(glm::mat4& projection, glm::mat4& view, bool displayViewTransformGuizmo);


	private:
		SystemMonitor m_sysMonitor{};

		std::shared_ptr<Entity> m_rootEntity{};

		std::shared_ptr<Entity> m_selectedEntity{};

		std::function<void(std::shared_ptr<Entity>)> m_onSelectionChanged; // << callback

		std::function<void(std::string, SceneSetting)> m_onSceneSettingChanged; // << callback

		void renderTabSettings();
		void renderTabAbout();
		void renderHierarchyWidget();
		void renderPropertiesWidget();

		void displayEntityHierarchy(const std::shared_ptr<Entity>& entity);
		void displayEntityDetails(const std::shared_ptr<Entity>& entity);


		ImVec4 getEntityColor(const engine::EntityType entityType);



		void renderComponents(const std::shared_ptr<Entity>& entity);


		void renderTransformComponent(const std::shared_ptr<Entity>& entity);
		void renderLightComponent(std::shared_ptr<LightComponent>& component);
		void renderCameraComponent(std::shared_ptr<CameraComponent>& component);
		void renderPrimitiveComponent(std::shared_ptr<PrimitiveComponent>& component);
		void renderModelComponent(std::shared_ptr<ModelComponent>& component);
		void renderAnimatorComponent(std::shared_ptr<AnimatorComponent>& component);
		void renderParticleSystemComponent(std::shared_ptr<ParticleSystemComponent>& component);
		void renderTerrainComponent(std::shared_ptr<TerrainComponent>& component);

		void updateTransformComponent(std::shared_ptr<TransformComponent>& transformComponent, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);


		

	protected:
		int sceneSetting_renderMethod{ static_cast<int>(DEFAULT_RENDER_METHOD) };
		bool sceneSetting_drawAsWireframe{ false };
		float sceneSetting_exposure{ DEFAULT_EXPOSURE };
		bool sceneSetting_enableGammaCorrection{ DEFAULT_ENABLE_GAMMA_CORRECTION };
		bool sceneSetting_enableToneMapping{ DEFAULT_ENABLE_TONE_MAPPING };
		int sceneSetting_applyPostProcessFx{ static_cast<int>(DEFAULT_POST_PROCESSING_FX) };
		bool sceneSetting_enableFaceCulling{ true };
		bool sceneSetting_enableCameraFrustrumCulling{ true };
		bool sceneSetting_drawLightsVisualHelpers{ false };
		bool sceneSetting_drawBoundingBoxesVisualHelpers{ false };
		bool sceneSetting_drawDebugNormalsVisualHelpers{ false };
		bool sceneSetting_enableShadows{ DEFAULT_ENABLE_SHADOWS };
		int sceneSetting_shadowCalculationMethod{ static_cast<int>(DEFAULT_SHADOWS_METHOD) };
		float sceneSetting_shadowIntensity{ DEFAULT_SHADOWS_INTENSITY };
		int sceneSetting_shadowMapTextureSize{ static_cast<int>(DEFAULT_SHADOWMAP_TEXTURE_SIZE) };
		float sceneSetting_shadowMapBiasFactor{ DEFAULT_SHADOW_MAPS_BIAS };
		float sceneSetting_shadowMapBlur{ DEFAULT_SHADOWS_BLUR };

		float sceneSetting_iblDiffuseIntensity{ DEFAULT_PBR_IBL_DIFFUSE_INTENSITY };
		float sceneSetting_iblSpecularIntensity{ DEFAULT_PBR_IBL_SPECULAR_INTENSITY };


		// guizmo
		float viewWidth{ 10.0f }; // for orthographic
		const float camYAngle{ 165.f / 180.f * 3.14159f };
		const float camXAngle{ 32.f / 180.f * 3.14159f };
		float camDistance{};
		int gizmoCount{ 1 };

		bool firstFrame{ true };
		int lastUsing{};


		std::shared_ptr<Camera> m_guizmoCamera{};
	};
}
#endif