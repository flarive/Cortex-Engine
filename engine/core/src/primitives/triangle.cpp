#include "../../include/primitives/triangle.h"

#include "../../include/managers/log_manager.h"
#include "../../include/singleton.h"
#include "../../include/uvmapping.h"

engine::Triangle::Triangle(const glm::vec3& position)
    : Primitive(position)
{
    logger.trace("Triangle constructor called");
}

engine::Triangle::Triangle(float width, float height, const glm::vec3& position)
    : Primitive(position), m_width(width ), m_height(height)
{
    logger.trace("Triangle constructor called");
}

void engine::Triangle::setup()
{
    geometrySetup();
}

void engine::Triangle::setup(const std::shared_ptr<Material>& material)
{
    Primitive::setMaterial(material);

    const UvMapping uv{};
    setup(material, uv);
}

void engine::Triangle::setup(const std::shared_ptr<Material>& material,
    const UvMapping& uv)
{
    Primitive::setMaterial(material);
    m_uvScale = uv.getUvScale();

    geometrySetup();

    if (material && material->hasTextureMap())
        material->loadTexturesAsync(true);
}

void engine::Triangle::geometrySetup()
{
    std::vector<Vertex> vertices = generateVertices();

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // UV
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

std::vector<engine::Vertex> engine::Triangle::generateVertices()
{
    return Primitive::generateTriangleVertices(1.0f, 1.0f, m_uvScale);
}


void engine::Triangle::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform)
{
    if (!m_isEnabled)
        return;

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
    OpenGLDebug::checkGLError("shader.use.triangle");

    setTransform(localTransform.getLocalPosition(), localTransform.getLocalRotation(), localTransform.getLocalScale());

    if (type == ShaderType::BlinnPhong || type == ShaderType::PBR)
    {
        if (!m_material->bind(shader)) {
            std::cerr << "Failed to bind textures. Skipping draw." << std::endl;
            return;
        }

        handleOpacity(shader);
        setShaderCommonUniforms(shader, type);
    }

    // used by all shaders (blinnphong, pbr, simpleDepthBuffer1, simpleDepthBuffer2)
    shader.setMat4("model", transformMatrix);

    if (type == ShaderType::BlinnPhong || type == ShaderType::PBR)
    {
        shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(transformMatrix))));
        shader.setBool("hasTangents", true);
        shader.setBool("isAnimated", false);
        shader.setBool("isTessellated", isTessellated());
    }

    // Send to GPU
    glBindVertexArray(m_VAO);
    OpenGLDebug::checkGLError("glBindVertexArray.triangle");

    bool wasFaceCullingEnabled = RenderState::isFaceCullingEnabled();

    // disable face culling for transparent primitives (to see back faces)
    if (m_material->isTransparent() || m_material->hasTransmission())
        RenderState::disableFaceCulling();

    glDrawArrays(GL_TRIANGLES, 0, 36);
    OpenGLDebug::checkGLError("glDrawArrays.triangle");

    // re enable face culling for transparent primitives if face culling was enabled
    if (wasFaceCullingEnabled)
        RenderState::enableFaceCulling();
    else
        RenderState::disableFaceCulling();

    glBindVertexArray(0);
    OpenGLDebug::checkGLError("glBindVertexArray.triangle");

    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    SceneSettings& sceneSettings = singleton->sceneSettings();

    if (m_material && (type == ShaderType::BlinnPhong || type == ShaderType::PBR))
    {
        m_material->unbind(); // Unbind textures to prevent OpenGL state retention
        OpenGLDebug::checkGLError("Unbind.triangle");
    }

    // restore opacity defaults
    RenderState::setOpaque();
}

void engine::Triangle::clean()
{
}

engine::Triangle::~Triangle()
{
    logger.trace("Triangle destructor called");
}
