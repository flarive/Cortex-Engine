#include "../../include/primitives/sphere.h"

#include "../../include/vertex.h"
#include "../../include/tools/helpers.h"

void engine::Sphere::setup(const std::shared_ptr<Material>& material)
{
    m_material = material;
    const UvMapping uv{};

    setup(material, uv);
}

void engine::Sphere::setup(const std::shared_ptr<Material>& material, const UvMapping& uv)
{
    m_material = material;
    m_uvScale = uv.getUvScale();

    setup();

    if (material && material->hasDiffuseMap())
        material->loadTexturesAsync();
}

void engine::Sphere::setup()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    // === Step 1: Generate vertex data ===
    std::vector<engine::Vertex> vertices = generateVertices();

    // === Step 2: Generate index data ===
    constexpr unsigned int X_SEGMENTS = 64;
    constexpr unsigned int Y_SEGMENTS = 64;
    std::vector<unsigned int> indices;
    indices.reserve(Y_SEGMENTS * (X_SEGMENTS + 1) * 2);

    bool oddRow = false;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow)
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                indices.push_back(y * (X_SEGMENTS + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        }
        else
        {
            for (int x = X_SEGMENTS; x >= 0; --x)
            {
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
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

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

    // Texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

    // Tangent attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));

    // Bitangent attribute
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)(11 * sizeof(float)));

    glBindVertexArray(0);
}


std::vector<engine::Vertex> engine::Sphere::generateVertices()
{
    return generateSphereVertices(radius, m_uvScale);
}


void engine::Sphere::draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation)
{
    shader.use();
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

    auto normalizedRotation = engine::Helpers::normalizeRotation(rotation);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    if (normalizedRotation.angle != 0) model = glm::rotate(model, glm::radians(normalizedRotation.angle), normalizedRotation.axis);
    model = glm::scale(model, size);
    shader.setMat4("model", model);
    shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
    shader.setBool("hasTangents", true);

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    m_material->unbind();
}

void engine::Sphere::draw(Shader& shader, const glm::mat4 transformMatrix)
{
    shader.use();
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

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    m_material->unbind();
}