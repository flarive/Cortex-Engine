#pragma once

#include "debug_draw_line.h"

namespace engine
{
	class DebugPlaneGrid
	{
	public:
		DebugPlaneGrid() = default;
		~DebugPlaneGrid() = default;

		void init(int size = 10, float spacing = 1.0f);
		void draw(const glm::mat4& projection, const glm::mat4& view);

	private:
		DebugDraw m_debugDrawLine{};
	};
}