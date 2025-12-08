#include "../../include/debug/debug_plane_grid.h"

void engine::DebugPlaneGrid::init(int size, float spacing)
{
	m_debugDrawLines.reserve(size * 2 + 2);
	float halfSize = (size * spacing) / 2.0f;
	// Create lines along the X axis
	for (int i = -size; i <= size; ++i) {
		DebugDraw line;
		glm::vec3 start(-halfSize, 0.0f, i * spacing);
		glm::vec3 end(halfSize, 0.0f, i * spacing);
		line.init();
		line.addLine(start, end, glm::vec3(1.0f, 0.0f, 0.0f));
		m_debugDrawLines.push_back(line);
	}
	// Create lines along the Z axis
	for (int i = -size; i <= size; ++i) {
		DebugDraw line;
		glm::vec3 start(i * spacing, 0.0f, -halfSize);
		glm::vec3 end(i * spacing, 0.0f, halfSize);
		line.init();
		line.addLine(start, end, glm::vec3(1.0f, 0.0f, 0.0f));
		m_debugDrawLines.push_back(line);
	}
}

void engine::DebugPlaneGrid::draw(const glm::mat4& projection, const glm::mat4& view)
{
	for (auto& line : m_debugDrawLines)
	{
		line.render(view, projection);
		line.clear();
	}
}
