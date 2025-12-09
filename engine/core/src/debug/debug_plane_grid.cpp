#include "../../include/debug/debug_plane_grid.h"

void engine::DebugPlaneGrid::init(int size, float spacing)
{
	m_debugDrawLine.init();

	

	float halfSize = (size * spacing) / 2.0f;

	glm::vec3 color{ 0.8f, 0.8f, 0.8f };

	// Create lines along the X axis
	for (int i = -size; i <= size; ++i)
	{
		glm::vec3 start(-halfSize, 0.0f, i * spacing);
		glm::vec3 end(halfSize, 0.0f, i * spacing);
		m_debugDrawLine.addLine(start, end, color);
	}
	// Create lines along the Z axis
	for (int i = -size; i <= size; ++i)
	{
		glm::vec3 start(i * spacing, 0.0f, -halfSize);
		glm::vec3 end(i * spacing, 0.0f, halfSize);
		m_debugDrawLine.addLine(start, end, color);
	}
}

void engine::DebugPlaneGrid::draw(const glm::mat4& projection, const glm::mat4& view)
{
	m_debugDrawLine.render(view, projection);
}
