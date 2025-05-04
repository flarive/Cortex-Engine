#pragma once

#include "../tools/system_monitor.h"

#include "../entity.h"

namespace engine
{
	class ImGuiDebug
	{
	public:
		ImGuiDebug() = default;
		~ImGuiDebug() = default;

		void setScene(std::shared_ptr<Entity> rootEntity);

		void renderUIWindow(bool show);

	private:
		SystemMonitor m_sysMonitor{};

		std::shared_ptr<Entity> m_rootEntity{};

		std::shared_ptr<Entity> m_selectedEntity{};

		void renderTabAbout();
		void renderTabScene();

		void displayEntityInImGui(const std::shared_ptr<Entity>& entity);
		void displayEntityDetails(const std::shared_ptr<Entity>& entity);
	};
}

