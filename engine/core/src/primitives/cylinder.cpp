#include "../../include/primitives/cylinder.h"

#include "../../include/vertex.h"
#include "../../include/tools/helpers.h"
#include "../../include/managers/log_manager.h"

engine::Cylinder::Cylinder(float _radius, float _height, const glm::vec3& _position) : Primitive(_position), m_radius(_radius), m_height(_height)
{
    logger.trace("Cylinder constructor called");
}

void engine::Cylinder::setup()
{
    geometrySetup(); // Geometry setup
}

void engine::Cylinder::setup(const std::shared_ptr<Material>& material)
{
    Primitive::setMaterial(material);

    const UvMapping uv{};
    setup(material, uv);
}

void engine::Cylinder::setup(const std::shared_ptr<Material>& material, const UvMapping& uv)
{
    Primitive::setMaterial(material);
    m_uvScale = uv.getUvScale();

    geometrySetup();

    if (material && material->hasTextureMap())
        material->loadTexturesAsync(true);
}

void engine::Cylinder::geometrySetup()
{
    // === Step 1: Generate vertex data ===
    m_vertices.clear();
    m_indices.clear();

    m_vertices = generateVertices();

    // === Step 2: Generate index data ===
    const unsigned int sectorCount = 36;

    // Side indices (CCW)
    for (unsigned int i = 0; i < sectorCount; ++i)
    {
        unsigned int k1 = i * 2;
        unsigned int k2 = k1 + 1;
        unsigned int k3 = ((i + 1) % sectorCount) * 2;
        unsigned int k4 = k3 + 1;

        m_indices.push_back(k1);
        m_indices.push_back(k2);
        m_indices.push_back(k3);

        m_indices.push_back(k3);
        m_indices.push_back(k2);
        m_indices.push_back(k4);
    }

    // Top cap indices
    unsigned int topCenterIndex =
        static_cast<unsigned int>(m_vertices.size() - (sectorCount + 1) - (sectorCount + 2));

    for (unsigned int i = 0; i < sectorCount; ++i)
    {
        m_indices.push_back(topCenterIndex);
        m_indices.push_back(topCenterIndex + i + 2);
        m_indices.push_back(topCenterIndex + i + 1);
    }

    // Bottom cap indices
    unsigned int bottomCenterIndex =
        static_cast<unsigned int>(m_vertices.size() - (sectorCount + 1));

    for (unsigned int i = 0; i < sectorCount; ++i)
    {
        m_indices.push_back(bottomCenterIndex);
        m_indices.push_back(bottomCenterIndex + i + 1);
        m_indices.push_back(bottomCenterIndex + i + 2);
    }

    m_indexCount = static_cast<unsigned int>(m_indices.size());

    // === Step 3: Send to GPU ===
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // TexCoords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    // Tangent
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(3);

    // Bitangent
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);
}

std::vector<engine::Vertex> engine::Cylinder::generateVertices()
{
    return generateCylinderVertices(36, m_height, m_radius, m_uvScale);
}

void engine::Cylinder::clean()
{

}

engine::Cylinder::~Cylinder()
{
    logger.trace("Cylinder destructor called");
}