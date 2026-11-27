#include "../../include/primitives/cube.h"

#include "../../include/vertex.h"
#include "../../include/uvmapping.h"
#include "../../include/materials/material.h"
#include "../../include/tools/helpers.h"

#include "../../include/singleton.h"
#include "../../include/managers/log_manager.h"

#include <vector>
//#include <glm/glm.hpp>

engine::Cube::Cube(const glm::vec3& _position)
    : Primitive(_position)
{
    logger.trace("Cube constructor called");
}

engine::Cube::Cube(const float& _size, const glm::vec3& _position)
    : Primitive(_position), m_width(_size), m_height(_size), m_depth(_size)
{
    logger.trace("Cube constructor called");
}

engine::Cube::Cube(const float& _width, const float& _height, const float& _depth, const glm::vec3& _position)
    : Primitive(_position), m_width(_width), m_height(_height), m_depth(_depth)
{
    logger.trace("Cube constructor called");
}

void engine::Cube::setup()
{
    geometrySetup(); // Geometry setup
}

void engine::Cube::setup(const std::shared_ptr<Material>& material)
{
    Primitive::setMaterial(material);

    const UvMapping uv{};
    setup(material, uv);
}

void engine::Cube::setup(const std::shared_ptr<Material>& material, const UvMapping& uv)
{
    Primitive::setMaterial(material);
    m_uvScale = uv.getUvScale();

    geometrySetup(); // Geometry setup

    if (material && material->hasTextureMap())
        material->loadTexturesAsync(true);
}

void engine::Cube::geometrySetup()
{
    // === Step 1: Generate vertex data ===
    m_vertices.clear();
    m_indices.clear();

    m_vertices = generateVertices();

    // Send to GPU
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    // Tangent attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(3);

    // Bitangent attribute
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);
}

std::vector<engine::Vertex> engine::Cube::generateVertices()
{
    return generateCuboidVertices(m_width, m_height, m_depth, m_uvScale);
}

void engine::Cube::drawDebugNormals(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix)
{
    m_debugDrawLine.init();

    // Define the local normals for each face of the cube
    const std::vector<glm::vec3> localNormals = {
        glm::vec3(1.0f, 0.0f, 0.0f),   // +X (right)
        glm::vec3(-1.0f, 0.0f, 0.0f),  // -X (left)
        glm::vec3(0.0f, 1.0f, 0.0f),   // +Y (top)
        glm::vec3(0.0f, -1.0f, 0.0f),  // -Y (bottom)
        glm::vec3(0.0f, 0.0f, 1.0f),   // +Z (front)
        glm::vec3(0.0f, 0.0f, -1.0f)   // -Z (back)
    };

    // Define the local centers for each face of the cube (assuming cube is axis-aligned and centered at origin)
    const std::vector<glm::vec3> localFaceCenters = {
        glm::vec3(0.5f, 0.0f, 0.0f),   // +X face center
        glm::vec3(-0.5f, 0.0f, 0.0f),  // -X face center
        glm::vec3(0.0f, 0.5f, 0.0f),   // +Y face center
        glm::vec3(0.0f, -0.5f, 0.0f),  // -Y face center
        glm::vec3(0.0f, 0.0f, 0.5f),   // +Z face center
        glm::vec3(0.0f, 0.0f, -0.5f)   // -Z face center
    };

    // Transform each face center and normal to world space
    for (size_t i = 0; i < localNormals.size(); ++i) {
        // Transform the face center to world space
        glm::vec3 worldFaceCenter = glm::vec3(transformMatrix * glm::vec4(localFaceCenters[i], 1.0f));
        // Transform the normal to world space
        glm::vec3 worldNormal = glm::normalize(glm::mat3(transformMatrix) * localNormals[i]);
        // Compute the end point of the normal line
        glm::vec3 end = worldFaceCenter + worldNormal * DEBUG_DRAW_NORMAL_LENGTH; // Scale for visibility
        // Add the debug line (with arrow)
        m_debugDrawLine.addLine(worldFaceCenter, end, DEBUG_DRAW_NORMAL_COLOR, DEBUG_DRAW_NORMAL_DRAW_ARROW, DEBUG_DRAW_NORMAL_ARROW_SIZE);
    }

    // Render all debug lines
    m_debugDrawLine.render(view, projection);
    m_debugDrawLine.clear();
}

void engine::Cube::clean()
{
    if (m_debugDrawLine.isInitialized())
        m_debugDrawLine.clean();
}

engine::Cube::~Cube()
{
    logger.trace("Cube destructor called");
}