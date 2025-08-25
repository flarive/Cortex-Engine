#pragma once

#include "../tools/system_monitor.h"

#include "../entity.h"

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

		void drawTransformEditor(Transform& transform);

		void drawLightEntityDetails(const std::shared_ptr<Entity>& entity);


	protected:
		std::unordered_map<EntityType, GLuint> m_iconSmallTextureCache;
		std::unordered_map<EntityType, GLuint> m_iconMediumTextureCache;

		std::unordered_map<std::string, GLuint> m_iconActionTextureCache;

		float itemWidth = 40.0f; // pixels


		bool settings_wireframe{};
	};
}