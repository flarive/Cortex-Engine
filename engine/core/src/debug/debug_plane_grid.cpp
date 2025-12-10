#include "../../include/debug/debug_plane_grid.h"

void engine::DebugPlaneGrid::init(int size, float spacing, const glm::vec3& offset)
{
    m_debugDrawLine.init();

    // Calculate the total size of the grid in world units
    float totalSize = size * spacing;
    float halfSize = totalSize / 2.0f;

    glm::vec3 color{ 0.6f };

    // Calculate the number of lines and their positions
    int numLines = size + 1; // 11 lines for size=10
    int start = -(size / 2);
    int end = (size / 2);

    // Create lines along the X axis (horizontal lines)
    for (int i = start; i <= end; ++i)
    {
        glm::vec3 startPos(-halfSize, 0.0f, i * spacing);
        glm::vec3 endPos(halfSize, 0.0f, i * spacing);

        // Apply the offset to both start and end positions
        startPos += offset;
        endPos += offset;

        m_debugDrawLine.addLine(startPos, endPos, color);
    }

    // Create lines along the Z axis (vertical lines)
    for (int i = start; i <= end; ++i)
    {
        glm::vec3 startPos(i * spacing, 0.0f, -halfSize);
        glm::vec3 endPos(i * spacing, 0.0f, halfSize);

        // Apply the offset to both start and end positions
        startPos += offset;
        endPos += offset;

        m_debugDrawLine.addLine(startPos, endPos, color);
    }
}

void engine::DebugPlaneGrid::draw(const glm::mat4& projection, const glm::mat4& view)
{
	m_debugDrawLine.render(view, projection);
}
