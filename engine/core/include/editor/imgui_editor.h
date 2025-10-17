#pragma once

#include "../tools/system_monitor.h"

#include "../ecs/entity.h"
#include "../ecs/transform_component.h"
#include "../ecs/light_component.h"
#include "../ecs/camera_component.h"
#include "../ecs/model_component.h"
#include "../ecs/primitive_component.h"

#include <imgui.h>

#if EDITOR_MODE
namespace engine
{
	class ImGuiEditor
	{
	public:
		ImGuiEditor() = default;
		~ImGuiEditor() = default;

		void setScene(std::shared_ptr<Entity> rootEntity);

		void renderUIWindow(bool show);

		// Let parent register a callback
		void setOnSelectionChanged(std::function<void(std::shared_ptr<Entity>)> callback) {
			m_onSelectionChanged = std::move(callback);
		}

		// Let parent register a callback
		void setOnSceneSettingChanged(std::function<void(std::string, bool)> callback) {
			m_onSceneSettingChanged = std::move(callback);
		}


	private:
		SystemMonitor m_sysMonitor{};

		std::shared_ptr<Entity> m_rootEntity{};

		std::shared_ptr<Entity> m_selectedEntity{};


		std::function<void(std::shared_ptr<Entity>)> m_onSelectionChanged; // << callback

		std::function<void(std::string, bool)> m_onSceneSettingChanged; // << callback

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


		void renderTransformComponent(const std::shared_ptr<Entity>& entity, bool displayPosition = true, bool displayRotation = true, bool displayScale = true);
		void renderLightComponent(std::shared_ptr<LightComponent>& component);
		void renderCameraComponent(std::shared_ptr<CameraComponent>& component);
		void renderPrimitiveComponent(std::shared_ptr<PrimitiveComponent>& component, std::shared_ptr<TransformComponent>& transformComponent);
		void renderModelComponent(std::shared_ptr<ModelComponent>& component);

		bool drawCustomDragFloat(const char* text, const char* name, const ImVec2& position, const ImVec2& size, float rounding, float width, ImU32 backgroundColor, ImU32 foregroundColor, float* value, float step);
		void drawCustomLabel(const char* text, const ImVec2& position, const ImVec2& size, float rounding, ImU32 backgroundColor, ImU32 foregroundColor);

		void updateTransformComponent(std::shared_ptr<TransformComponent>& transformComponent, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);


	protected:
		std::unordered_map<EntityType, GLuint> m_iconSmallTextureCache{};
		std::unordered_map<EntityType, GLuint> m_iconMediumTextureCache{};

		std::unordered_map<std::string, GLuint> m_iconActionTextureCache{};

		float itemLabelWidth{ 100.0f }; // pixels

		bool sceneSetting_drawAsWireframe{ false };
		bool sceneSetting_enableGammaCorrection{ false };
		bool sceneSetting_enableFaceCulling{ true };
		bool sceneSetting_enableCameraFrustrumCulling{ true };
		bool sceneSetting_drawLightsVisualHelpers{ false };
		bool sceneSetting_drawBoundingBoxesVisualHelpers{ false };
		bool sceneSetting_drawDebugNormalsVisualHelpers{ false };

		const float ROUNDING{ 3.0f };
		const ImVec2 SIZE{ ImVec2(21, 21) };
	};
}
#endif