#include "../../include/primitives/billboard.h"

#include "../../include/vertex.h"
#include "../../include/uvmapping.h"
#include "../../include/tools/helpers.h"
#include "../../include/managers/log_manager.h"

engine::Billboard::Billboard(const glm::vec3& _position) : Primitive(_position)
{
    logger.trace("Billboard constructor called");

    setCanCastShadows(false);
	setCanReceiveShadows(false);
}

void engine::Billboard::setup()
{
    geometrySetup(); // Geometry setup
}

void engine::Billboard::setup(const std::shared_ptr<Material>& material)
{
    Primitive::setMaterial(material);

    const UvMapping uv{};
    setup(material, uv);
}

void engine::Billboard::setup(const std::shared_ptr<Material>& material, const UvMapping& uv)
{
    Primitive::setMaterial(material);
    m_uvScale = uv.getUvScale();

    geometrySetup(); // Geometry setup

    if (material && material->hasTextureMap())
        material->loadTexturesAsync(true);
}

void engine::Billboard::geometrySetup()
{
    if (!m_isEnabled)
		return;

    // === Step 1: Generate vertex data ===
    m_vertices.clear();
    m_indices.clear();

    m_vertices = generateVertices();
    
    // === Send to GPU ===

    // configure plane VAO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    // Send to GPU
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

std::vector<engine::Vertex> engine::Billboard::generateVertices()
{
    return generateBillboardVertices(m_uvScale);
}

void engine::Billboard::clean()
{

}

engine::Billboard::~Billboard()
{
    logger.trace("Billboard destructor called");
}
