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


	private:
		SystemMonitor m_sysMonitor{};

		std::shared_ptr<Entity> m_rootEntity{};

		std::shared_ptr<Entity> m_selectedEntity{};




		void renderTabAbout();
		void renderHierarchyWidget();
		void renderPropertiesWidget();

		void displayEntityInImGui(const std::shared_ptr<Entity>& entity);
		void displayEntityDetails(const std::shared_ptr<Entity>& entity);

		GLuint getEntityTypeIcon(const engine::EntityType entityType);
		ImVec4 getEntityColor(const engine::EntityType entityType);


	protected:
		std::unordered_map< engine::EntityType, GLuint> m_iconTextureCache;


	};
}

