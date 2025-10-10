#include "../../include/primitives/plane.h"

#include "../../include/vertex.h"
#include "../../include/uvmapping.h"
#include "../../include/primitives/primitive.h"
#include "../../include/tools/helpers.h"

engine::Plane::Plane(const glm::vec3& _position) : Primitive(_position)
{
}

void engine::Plane::setup(const std::shared_ptr<Material>& material)
{
    m_material = material; // Store material reference

    const UvMapping uv{};
    setup(material, uv);
}

void engine::Plane::setup(const std::shared_ptr<Material>& material, const UvMapping& uv)
{
    m_material = material;
    m_uvScale = uv.getUvScale();

    setup(); // Geometry setup

    if (material && material->hasDiffuseMap())
        material->loadTexturesAsync(); // Let material handle texture loading
}

void engine::Plane::setup()
{
    std::vector<Vertex> vertices = generatePlaneVertices(m_uvScale);

    // configure plane VAO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    
    // Send to GPU
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

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
}

std::vector<engine::Vertex> engine::Plane::generateVertices()
{
    return generatePlaneVertices(m_uvScale);
}

void engine::Plane::draw(Shader& shader, const glm::mat4& transformMatrix, Transform& localTransform)
{
    shader.use();

    position = localTransform.getLocalPosition();
    rotation = localTransform.getLocalRotation();
    scale = localTransform.getLocalScale();

    if (m_material)
    {
        m_material->bind(shader);
        shader.setVec3("material.ambient_color", m_material->getAmbientColor());
        shader.setVec3("material.diffuse_color", m_material->getDiffuseColor());
        shader.setVec3("material.specular_color", m_material->getSpecularColor());

        shader.setFloat("material.shininess", m_material->getShininessIntensity());

        shader.setFloat("material.ambient_intensity", m_material->getAmbientIntensity());


        shader.setFloat("material.heightScale", m_material->getHeightIntensity());
        shader.setFloat("material.normalMapIntensity", m_material->getNormalIntensity());
        shader.setFloat("material.emissiveIntensity", m_material->getEmissiveIntensity());
    }

    shader.setMat4("model", transformMatrix);
    shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(transformMatrix))));
    shader.setBool("hasTangents", true);

    // Send to GPU
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    m_material->unbind(); // Unbind textures to prevent OpenGL state retention
}
