#include "../../include/primitives/cone.h"

#include "../../include/vertex.h"
#include "../../include/tools/helpers.h"


engine::Cone::Cone(float _radius, float _height, const glm::vec3& _position) : Primitive(_position), m_radius(_radius), m_height(_height)
{
}

void engine::Cone::setup()
{
    geometrySetup(); // Geometry setup
}

void engine::Cone::setup(const std::shared_ptr<Material>& material)
{
    m_material = material;

    const UvMapping uv{};
    setup(material, uv);
}

void engine::Cone::setup(const std::shared_ptr<Material>& material, const UvMapping& uv)
{
    m_material = material;
    m_uvScale = uv.getUvScale();

    geometrySetup();

    if (material && material->hasDiffuseMap())
        material->loadTexturesAsync();
}

void engine::Cone::geometrySetup()
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
        unsigned int baseIndex = i + 1;
        unsigned int nextIndex = (i + 1) % sectorCount + 1;
        indices.push_back(0);           // tip
        indices.push_back(nextIndex);   // next base
        indices.push_back(baseIndex);   // current base
    }

    // Base indices
    unsigned int baseCenterIndex = static_cast<unsigned int>(vertices.size() - (sectorCount + 1));
    for (unsigned int i = 0; i < sectorCount; ++i)
    {
        indices.push_back(baseCenterIndex);             // center
        indices.push_back(baseCenterIndex + i + 1);     // current
        indices.push_back(baseCenterIndex + i + 2);     // next
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

    // === Step 4: Send to GPU ===
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    constexpr unsigned int stride = 14 * sizeof(float);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    // Tangent attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(3);

    // Bitangent attribute
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, bitangent));
    glEnableVertexAttribArray(4);

    // Bones ids (not really needed here)
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIDs));
    glEnableVertexAttribArray(5);

    // Bones weights (not really needed here)
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));
    glEnableVertexAttribArray(6);

    glBindVertexArray(0);
}


std::vector<engine::Vertex> engine::Cone::generateVertices()
{
    return generateConeVertices(36, m_height, m_radius, m_uvScale);
}

void engine::Cone::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform)
{
    ShaderType type = shader.getShaderType();

    if (!m_material || !shader.isValid()) {
        std::cerr << "Material or shader not valid. Skipping draw." << std::endl;
        return;
    }

    if (!m_material->areAllTexturesLoaded()) {
        std::cout << "Textures not ready. Deferring draw." << std::endl;
        return;
    }

    if (m_VAO == 0 || m_VBO == 0) {
        std::cerr << "VAO/VBO not initialized. Skipping draw." << std::endl;
        return;
    }

    shader.use();
    OpenGLDebug::checkGLError("shader.use44");

    setTransform(localTransform.getLocalPosition(), localTransform.getLocalRotation(), localTransform.getLocalScale());

    if (m_material)
    {
        if (type == ShaderType::BlinnPhong || type == ShaderType::PBR)
        {
            if (!m_material->bind(shader)) {
                std::cerr << "Failed to bind textures. Skipping draw." << std::endl;
                return;
            }

            shader.setVec3("material.ambient_color", m_material->getAmbientColor());
            shader.setVec3("material.diffuse_color", m_material->getDiffuseColor());
            shader.setVec3("material.specular_color", m_material->getSpecularColor());

            shader.setFloat("material.shininess", m_material->getShininessIntensity());

            shader.setFloat("material.ambient_intensity", m_material->getAmbientIntensity());


            //shader.setFloat("material.heightScale", m_material->getHeightIntensity());
            shader.setFloat("material.normalMapIntensity", m_material->getNormalIntensity());
            shader.setFloat("material.emissiveIntensity", m_material->getEmissiveIntensity());

            shader.setBool("material.canCastShadows", canCastShadows());
            shader.setBool("material.canReceiveShadows", canReceiveShadows());
        }
    }

    // used by all shaders (blinnphong, pbr, simpleDepthBuffer1, simpleDepthBuffer2)
    shader.setMat4("model", transformMatrix);

    if (type == ShaderType::BlinnPhong || type == ShaderType::PBR)
    {
        shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(transformMatrix))));
        shader.setBool("hasTangents", true);
    }

    // Send to GPU
    glBindVertexArray(m_VAO);
    OpenGLDebug::checkGLError("glBindVertexArray");

    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    OpenGLDebug::checkGLError("glDrawArrays");

    glBindVertexArray(0);
    OpenGLDebug::checkGLError("glBindVertexArray");

    if (m_material && (type == ShaderType::BlinnPhong || type == ShaderType::PBR))
    {
        m_material->unbind(); // Unbind textures to prevent OpenGL state retention
        OpenGLDebug::checkGLError("Unbind");
    }
}

void engine::Cone::clean()
{

}
