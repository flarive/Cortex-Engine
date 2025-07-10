#include "../../include/primitives/cylinder.h"

#include "../../include/vertex.h"
#include "../../include/tools/helpers.h"

void engine::Cylinder::setup(const std::shared_ptr<Material>& material)
{
    m_material = material;

    const UvMapping uv{};

    setup(material, uv);
}

void engine::Cylinder::setup(const std::shared_ptr<Material>& material, const UvMapping& uv)
{
    m_material = material;
    m_uvScale = uv.getUvScale();

    setup();

    if (material)
        material->loadTexturesAsync();
}

void engine::Cylinder::setup()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    // === Step 1: Generate vertex data ===
    std::vector<engine::Vertex> vertices = generateVertices();

    // === Step 2: Generate index data ===
    std::vector<unsigned int> indices;
    const unsigned int sectorCount = 36;

    // Side indices
    for (unsigned int i = 0; i < sectorCount; ++i)
    {
        unsigned int k1 = i * 2;
        unsigned int k2 = k1 + 1;
        unsigned int k3 = ((i + 1) % sectorCount) * 2;
        unsigned int k4 = k3 + 1;

        indices.push_back(k1);
        indices.push_back(k3);
        indices.push_back(k2);

        indices.push_back(k2);
        indices.push_back(k3);
        indices.push_back(k4);
    }

    // Top cap indices
    unsigned int topCenterIndex = static_cast<unsigned int>(vertices.size() - (sectorCount + 1) - (sectorCount + 2));
    for (unsigned int i = 0; i < sectorCount; ++i)
    {
        indices.push_back(topCenterIndex);
        indices.push_back(topCenterIndex + i + 1);
        indices.push_back(topCenterIndex + i + 2);
    }

    // Bottom cap indices
    unsigned int bottomCenterIndex = static_cast<unsigned int>(vertices.size() - (sectorCount + 1));
    for (unsigned int i = 0; i < sectorCount; ++i)
    {
        indices.push_back(bottomCenterIndex);
        indices.push_back(bottomCenterIndex + i + 2);
        indices.push_back(bottomCenterIndex + i + 1);
    }

    indexCount = static_cast<unsigned int>(indices.size());

    // === Step 3: Interleave vertex data into a float buffer ===
    std::vector<float> data;
    data.reserve(vertices.size() * 14);

    for (const auto& v : vertices)
    {
        data.push_back(v.position.x);
        data.push_back(v.position.y);
        data.push_back(v.position.z);

        data.push_back(v.normal.x);
        data.push_back(v.normal.y);
        data.push_back(v.normal.z);

        data.push_back(v.texCoords.x);
        data.push_back(v.texCoords.y);

        data.push_back(v.tangent.x);
        data.push_back(v.tangent.y);
        data.push_back(v.tangent.z);

        data.push_back(v.bitangent.x);
        data.push_back(v.bitangent.y);
        data.push_back(v.bitangent.z);
    }

    // === Step 4: Upload to OpenGL ===
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    constexpr unsigned int stride = 14 * sizeof(float);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)(11 * sizeof(float)));

    glBindVertexArray(0);
}

std::vector<engine::Vertex> engine::Cylinder::generateVertices()
{
    return generateCylinderVertices(36, height, radius, m_uvScale);
}

void engine::Cylinder::draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation)
{
    shader.use();
    if (m_material)
    {
        m_material->bind(shader);
        shader.setVec3("material.ambient_color", m_material->getAmbientColor());
        shader.setVec3("material.diffuse_color", m_material->getDiffuseColor());
        shader.setVec3("material.specular_color", m_material->getSpecularColor());
        shader.setFloat("material.ambient_intensity", m_material->getAmbientIntensity());

        shader.setBool("hasTangents", true);

        shader.setFloat("material.heightScale", m_material->getHeightIntensity());
        shader.setFloat("material.normalMapIntensity", m_material->getNormalIntensity());
        shader.setFloat("material.emissiveIntensity", 0.0f);
    }

    auto normalizedRotation = engine::Helpers::normalizeRotation(rotation);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    if (normalizedRotation.angle != 0) model = glm::rotate(model, glm::radians(normalizedRotation.angle), normalizedRotation.axis);
    model = glm::scale(model, size);

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    m_material->unbind();
}


void engine::Cylinder::draw(Shader& shader, const glm::mat4 model)
{
    shader.use();
    if (m_material)
    {
        m_material->bind(shader);
        shader.setVec3("material.ambient_color", m_material->getAmbientColor());
        shader.setVec3("material.diffuse_color", m_material->getDiffuseColor());
        shader.setVec3("material.specular_color", m_material->getSpecularColor());
        shader.setFloat("material.ambient_intensity", m_material->getAmbientIntensity());
        shader.setBool("hasTangents", true);

        shader.setFloat("material.heightScale", m_material->getHeightIntensity());
        shader.setFloat("material.normalMapIntensity", m_material->getNormalIntensity());
        shader.setFloat("material.emissiveIntensity", 0.0f);
    }

    shader.setMat4("model", model);
    shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    m_material->unbind();
}
