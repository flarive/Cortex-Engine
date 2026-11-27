#include "../../include/primitives/sphere.h"

#include "../../include/vertex.h"
#include "../../include/tools/helpers.h"
#include "../../include/managers/log_manager.h"

engine::Sphere::Sphere(bool _flipNormals, const glm::vec3& _position) : m_flipNormals(_flipNormals), Primitive(_position)
{
    logger.trace("Sphere constructor called");
}

void engine::Sphere::setup()
{
    geometrySetup(); // Geometry setup
}

void engine::Sphere::setup(const std::shared_ptr<Material>& material)
{
    Primitive::setMaterial(material);

    const UvMapping uv{};
    setup(material, uv);
}

void engine::Sphere::setup(const std::shared_ptr<Material>& material, const UvMapping& uv)
{
    Primitive::setMaterial(material);

    m_uvScale = uv.getUvScale();

    geometrySetup();

    if (material && material->hasTextureMap())
        material->loadTexturesAsync(true);
}

void engine::Sphere::geometrySetup()
{
    // === Step 1: Generate vertex data ===
    m_vertices.clear();
    m_indices.clear();

    m_vertices = generateVertices();

    // === Step 2: Generate index data ===
    constexpr unsigned int X_SEGMENTS = 64;
    constexpr unsigned int Y_SEGMENTS = 64;

    m_indices.reserve(Y_SEGMENTS * (X_SEGMENTS + 1) * 2);

    bool oddRow = false;

    for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow)
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                m_indices.push_back(y * (X_SEGMENTS + 1) + x);
                m_indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        }
        else
        {
            for (int x = static_cast<int>(X_SEGMENTS); x >= 0; --x)
            {
                m_indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                m_indices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }

        oddRow = !oddRow;
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

std::vector<engine::Vertex> engine::Sphere::generateVertices()
{
    return generateSphereVertices(m_radius, m_uvScale);
}

void engine::Sphere::clean()
{
    if(m_EBO) { glDeleteBuffers(1, &m_EBO); m_EBO = 0; }
    if (m_VBO) { glDeleteBuffers(1, &m_VBO); m_VBO = 0; }
    if (m_VAO) { glDeleteVertexArrays(1, &m_VAO); m_VAO = 0; }
}

engine::Sphere::~Sphere()
{
    logger.trace("Sphere destructor called");
}