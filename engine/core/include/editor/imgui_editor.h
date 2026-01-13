#pragma once

#include "../tools/system_monitor.h"

#include "../ecs/entity.h"
#include "../ecs/transform_component.h"
#include "../ecs/light_component.h"
#include "../ecs/camera_component.h"
#include "../ecs/model_component.h"
#include "../ecs/primitive_component.h"
#include "../ecs/animator_component.h"

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

		void renderUIWindow(bool show, const float width, const float height, const bool fullscreen, const bool displayObjectTransformGuizmo, const bool displayViewTransformGuizmo);

		// Let parent register a callback
		void setOnSelectionChanged(std::function<void(std::shared_ptr<Entity>)> callback) {
			m_onSelectionChanged = std::move(callback);
		}

		// Let parent register a callback
		void setOnSceneSettingChanged(std::function<void(std::string, SceneSetting)> callback) {
			m_onSceneSettingChanged = std::move(callback);
		}


		void initRenderGuizmo(const std::shared_ptr<Camera> camera);

		void renderGuizmo(const ImGuiID& dockspace_id, const std::shared_ptr<Entity> selectedEntity, const std::shared_ptr<Camera> camera, const float width, const float height, const bool fullscreen, const bool displayObjectTransformGuizmo, const bool displayViewTransformGuizmo);

		void editTransform(const float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition, std::shared_ptr<Entity> entity, int windowX, int windowY, int windowWidth, int windowHeight);


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

		GLuint getEntityTypeSmallIcon(const EntityType entityType);
		GLuint getEntityTypeMediumIcon(const EntityType entityType);

		ImVec4 getEntityColor(const engine::EntityType entityType);

		GLuint getEntityActionIcon(const std::string& key);

		void renderComponents(const std::shared_ptr<Entity>& entity);


		void renderTransformComponent(const std::shared_ptr<Entity>& entity);
		void renderLightComponent(std::shared_ptr<LightComponent>& component);
		void renderCameraComponent(std::shared_ptr<CameraComponent>& component);
		void renderPrimitiveComponent(std::shared_ptr<PrimitiveComponent>& component);
		void renderModelComponent(std::shared_ptr<ModelComponent>& component);
		void renderAnimatorComponent(std::shared_ptr<AnimatorComponent>& component);

		void updateTransformComponent(std::shared_ptr<TransformComponent>& transformComponent, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);


		

	protected:
		std::unordered_map<EntityType, GLuint> m_iconSmallTextureCache{};
		std::unordered_map<EntityType, GLuint> m_iconMediumTextureCache{};

		std::unordered_map<std::string, GLuint> m_iconActionTextureCache{};

		

		

		bool sceneSetting_drawAsWireframe{ false };
		bool sceneSetting_enableGammaCorrection{ false };
		bool sceneSetting_enableFaceCulling{ true };
		bool sceneSetting_enableCameraFrustrumCulling{ true };
		bool sceneSetting_drawLightsVisualHelpers{ false };
		bool sceneSetting_drawBoundingBoxesVisualHelpers{ false };
		bool sceneSetting_drawDebugNormalsVisualHelpers{ false };
		bool sceneSetting_enableShadows{ true };
		int sceneSetting_shadowCalculationMethod{ static_cast<int>(ShadowCalculationMethod::PCFSoft) };
		float sceneSetting_shadowIntensity{ 1.5f };
		int sceneSetting_shadowMapTextureSize{ 2048 };
		float sceneSetting_shadowMapBiasFactor{ 0.001f };
		float sceneSetting_shadowMapBlur{ 1.0f };


		float viewWidth; // for orthographic
		const float camYAngle;
		const float camXAngle;
		float camDistance;
		int gizmoCount;

		bool firstFrame;
		int lastUsing;

		std::shared_ptr<Camera> m_guizmoCamera{};
	};
}
#endif