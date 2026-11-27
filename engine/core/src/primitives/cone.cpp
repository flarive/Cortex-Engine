#include "../../include/primitives/cone.h"

#include "../../include/vertex.h"
#include "../../include/tools/helpers.h"
#include "../../include/managers/log_manager.h"

#include "../../include/shaders/shader_ubo.h"

engine::Cone::Cone(float _radius, float _height, const glm::vec3& _position) : Primitive(_position), m_radius(_radius), m_height(_height)
{
    logger.trace("Cone constructor called");
}

void engine::Cone::setup()
{
    geometrySetup(); // Geometry setup
}

void engine::Cone::setup(const std::shared_ptr<Material>& material)
{
    Primitive::setMaterial(material);

    const UvMapping uv{};
    setup(material, uv);
}

void engine::Cone::setup(const std::shared_ptr<Material>& material, const UvMapping& uv)
{
    Primitive::setMaterial(material);
    m_uvScale = uv.getUvScale();

    geometrySetup();

    if (material && material->hasTextureMap())
        material->loadTexturesAsync(true);
}

void engine::Cone::geometrySetup()
{
    // === Step 1: Generate vertex data ===
    m_vertices.clear();
    m_indices.clear();

    m_vertices = generateVertices();

    // === Step 2: Generate index data ===
    constexpr unsigned int sectorCount = 36;

    // Side indices
    for (unsigned int i = 0; i < sectorCount; ++i)
    {
        const unsigned int baseIndex = i + 1;
        const unsigned int nextIndex = (i + 1) % sectorCount + 1;

        m_indices.push_back(0);          // tip
        m_indices.push_back(nextIndex);  // next base
        m_indices.push_back(baseIndex);  // current base
    }

    // Base indices
    const unsigned int baseCenterIndex =
        static_cast<unsigned int>(m_vertices.size() - (sectorCount + 1));

    for (unsigned int i = 0; i < sectorCount; ++i)
    {
        m_indices.push_back(baseCenterIndex);         // center
        m_indices.push_back(baseCenterIndex + i + 1);
        m_indices.push_back(baseCenterIndex + i + 2);
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

    // Texture coordinates
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

std::vector<engine::Vertex> engine::Cone::generateVertices()
{
    return generateConeVertices(36, m_height, m_radius, m_uvScale);
}

void engine::Cone::clean()
{

}

engine::Cone::~Cone()
{
    logger.trace("Cone destructor called");
}