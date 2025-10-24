#include "../../include/primitives/plane.h"

#include "../../include/vertex.h"
#include "../../include/uvmapping.h"
#include "../../include/primitives/primitive.h"

#include "../../include/singleton.h"


engine::Plane::Plane(bool _flipNormals, const glm::vec3& _position) : m_flipNormals(_flipNormals), Primitive(_position)
{
}

void engine::Plane::setup()
{
    geometrySetup(); // Geometry setup
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

    geometrySetup(); // Geometry setup

    if (material && material->hasDiffuseMap())
        material->loadTexturesAsync(); // Let material handle texture loading
}

void engine::Plane::geometrySetup()
{
    std::vector<Vertex> vertices = generateVertices();

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
    return generatePlaneVertices(m_uvScale, m_flipNormals);
}

void engine::Plane::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform)
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


    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    SceneSettings& sceneSettings = singleton->sceneSettings();

    if (sceneSettings.drawNormalsVisualHelpers) {
        drawDebugNormals(projection, view, transformMatrix);
    }

    m_material->unbind(); // Unbind textures to prevent OpenGL state retention
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
    glm::vec3 end = center + worldNormal * 0.5f; // Scale for visibility

    // Add the debug line
    m_debugDrawLine.addLine(center, end, glm::vec3(1.0f, 0.0f, 0.0f), true, 0.06f); // Red color for normal

    m_debugDrawLine.render(view, projection);
    m_debugDrawLine.clear();
}

void engine::Plane::clean()
{
    m_debugDrawLine.clean();
}
