#include "../../include/primitives/plane.h"

#include "../../include/vertex.h"
#include "../../include/uvmapping.h"
#include "../../include/primitives/primitive.h"

#include "../../include/singleton.h"
#include "../../include/managers/log_manager.h"


engine::Plane::Plane(bool _flipNormals, const glm::vec3& _position) : m_flipNormals(_flipNormals), Primitive(_position)
{
    logger.trace("Plane constructor called");
}

void engine::Plane::setup()
{
    geometrySetup(); // Geometry setup
}

void engine::Plane::setup(const std::shared_ptr<Material>& material)
{
    Primitive::setMaterial(material);

    const UvMapping uv{};
    setup(material, uv);
}

void engine::Plane::setup(const std::shared_ptr<Material>& material, const UvMapping& uv)
{
    Primitive::setMaterial(material);
    m_uvScale = uv.getUvScale();

    geometrySetup(); // Geometry setup

    if (material && material->hasTextureMap())
        material->loadTexturesAsync(true);
}

void engine::Plane::geometrySetup()
{
    // === Step 1: Generate vertex data ===
    m_vertices.clear();
    m_indices.clear();

    m_vertices = generateVertices();

    // Send to GPU

    // configure plane VAO
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

std::vector<engine::Vertex> engine::Plane::generateVertices()
{
    return Primitive::generatePlaneVertices(m_uvScale, m_flipNormals);
}

void engine::Plane::drawDebugNormals(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix)
{
    m_debugDrawLine.init();

    // Compute the center of the plane in world space
    glm::vec3 center = glm::vec3(transformMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    // Compute the normal in world space
    glm::vec3 localNormal = glm::vec3(0.0f, 1.0f, 0.0f); // Assuming plane normal is +Y in local space
    glm::vec3 worldNormal = glm::normalize(glm::mat3(transformMatrix) * localNormal);

    // Compute the end point of the normal line
    glm::vec3 end = center + worldNormal * DEBUG_DRAW_NORMAL_LENGTH; // Scale for visibility

    // Add the debug line
    m_debugDrawLine.addLine(center, end, DEBUG_DRAW_NORMAL_COLOR, DEBUG_DRAW_NORMAL_DRAW_ARROW, DEBUG_DRAW_NORMAL_ARROW_SIZE);

    m_debugDrawLine.render(view, projection);
    m_debugDrawLine.clear();
}

void engine::Plane::clean()
{
    if (m_debugDrawLine.isInitialized())
        m_debugDrawLine.clean();
}

engine::Plane::~Plane()
{
    logger.trace("Plane destructor called");
}