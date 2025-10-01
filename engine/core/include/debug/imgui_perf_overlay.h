#pragma once

namespace engine
{
	class ImGuiPerfOverlay
	{
	public:
		ImGuiPerfOverlay() = default;
		~ImGuiPerfOverlay() = default;

		void renderPerfOverlay(bool* p_open, const float& fps, const double& cpuTime, const double& gpuTime, const double& uiTime);
	};
}