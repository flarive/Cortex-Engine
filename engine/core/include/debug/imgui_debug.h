#pragma once

#include "../tools/system_monitor.h"

namespace engine
{
	class ImGuiDebug
	{
	public:
		ImGuiDebug() = default;
		
		void renderUIWindow(bool show);

	private:
		SystemMonitor sysMonitor{};
	};
}

