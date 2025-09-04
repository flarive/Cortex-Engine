#pragma once

#include "../tools/system_monitor.h"

#include "../ecs/entity.h"

#include <imgui.h>
#include <imgui_internal.h>



namespace engine
{
	class ImGuiDocking
	{
	public:
		ImGuiDocking() = default;
		~ImGuiDocking() = default;

		void setScene(std::shared_ptr<Entity> rootEntity);

		void renderUIWindow(bool show);

		// Let parent register a callback
		void setOnSelectionChanged(std::function<void(std::shared_ptr<Entity>)> callback) {
			m_onSelectionChanged = std::move(callback);
		}

		// Let parent register a callback
		void setOnRenderModeSettingChanged(std::function<void(bool)> callback) {
			m_onRenderModeSettingChanged = std::move(callback);
		}


	private:
		SystemMonitor m_sysMonitor{};

		std::shared_ptr<Entity> m_rootEntity{};

		std::shared_ptr<Entity> m_selectedEntity{};


		std::function<void(std::shared_ptr<Entity>)> m_onSelectionChanged; // << callback

		std::function<void(bool)> m_onRenderModeSettingChanged; // << callback

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

		void drawTransformEditor(engine::Transform& transform, bool position = true, bool rotation = true, bool scale = true);

		void drawLightEntityDetails(const std::shared_ptr<Entity>& entity);
		void drawCameraEntityDetails(const std::shared_ptr<Entity>& entity);

		void drawCustomDragFloat(const char* text, const char* name, const ImVec2& position, const ImVec2& size, float rounding, float width, ImU32 backgroundColor, ImU32 foregroundColor, float& value, float step);
		void drawCustomLabel(const char* text, const ImVec2& position, const ImVec2& size, float rounding, ImU32 backgroundColor, ImU32 foregroundColor);


	protected:
		std::unordered_map<EntityType, GLuint> m_iconSmallTextureCache;
		std::unordered_map<EntityType, GLuint> m_iconMediumTextureCache;

		std::unordered_map<std::string, GLuint> m_iconActionTextureCache;

		float itemLabelWidth = 100.0f; // pixels


		bool settings_wireframe{};

		const float ROUNDING = 3.0f;
		const ImVec2 SIZE = ImVec2(21, 21);
	};
}