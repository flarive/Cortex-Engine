#pragma once

#include <string>
#include <vector>

#include "../common_defines.h"

#include <imgui.h>

namespace engine
{
	class EditorHelper final
	{
	public:
		static void renderVectorTable(const std::vector<std::string>& items, const EditorProperty& property);

	private:
		static const float ROUNDING{ 3.0f };
		static const ImVec2 SIZE{ ImVec2(21, 21) };
	};
}