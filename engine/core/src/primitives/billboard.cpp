#include "../../include/primitives/billboard.h"

#include "../../include/vertex.h"
#include "../../include/uvmapping.h"
#include "../../include/tools/helpers.h"

engine::Billboard::Billboard(const glm::vec3& _position) : Primitive(_position)
{
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

    if (material && material->hasDiffuseMap())
        material->loadTexturesAsync(); // Let material handle texture loading
}

void engine::Billboard::geometrySetup()
{
    if (!m_isEnabled)
		return;

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

    glBindVertexArray(0);
}

std::vector<engine::Vertex> engine::Billboard::generateVertices()
{
    return generateBillboardVertices(m_uvScale);
}

void engine::Billboard::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform)
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
    OpenGLDebug::checkGLError("shader.use55");

    setTransform(localTransform.getLocalPosition(), localTransform.getLocalRotation(), localTransform.getLocalScale());

    if (m_material)
    {
        if (type == ShaderType::BlinnPhong || type == ShaderType::PBR)
        {
            if (!m_material->bind(shader)) {
                std::cerr << "Failed to bind textures. Skipping draw." << std::endl;
                return;
            }

            if (type == ShaderType::BlinnPhong)
            {
                shader.setFloat("material.shininess", m_material->getShininessIntensity());
                shader.setVec3("material.diffuse_color", m_material->getDiffuseColor());
                shader.setVec3("material.specular_color", m_material->getSpecularColor());
            }

            if (type == ShaderType::BlinnPhong || type == ShaderType::Parallax)
            {
                shader.setBool("material.useParallaxMapping", m_material->useParallaxMapping());
                shader.setFloat("material.heightScale", m_material->getParallaxIntensity());
            }

            shader.setBool("material.canCastShadows", canCastShadows());
            shader.setBool("material.canReceiveShadows", canReceiveShadows());

            if (type == ShaderType::PBR)
            {
                shader.setVec3("material.ambient_color", m_material->getAmbientColor());
                shader.setFloat("material.ambient_intensity", m_material->getAmbientIntensity());
            }
        }
    }

    glEnable(GL_BLEND);

    // Get camera's right and up vectors from the view matrix
    glm::mat4 viewInverse = glm::inverse(view);
    glm::vec3 cameraRight = glm::normalize(glm::vec3(viewInverse[0]));
    glm::vec3 cameraUp = glm::normalize(glm::vec3(viewInverse[1]));

    // Billboard's forward axis is the negative of the camera's forward axis
    glm::vec3 cameraForward = -glm::normalize(glm::vec3(viewInverse[2]));

    // Construct the billboard's model matrix
    glm::mat4 billboardModel = glm::mat4(1.0f);
    billboardModel[0] = glm::vec4(cameraRight, 0.0f);
    billboardModel[1] = glm::vec4(cameraUp, 0.0f);
    billboardModel[2] = glm::vec4(cameraForward, 0.0f);
    billboardModel[3] = glm::vec4(getPosition(), 1.0f);

    // Apply scale
    billboardModel = glm::scale(billboardModel, getScale());

    // Pass the billboard's model matrix to the shader
    shader.setMat4("model", billboardModel);

    if (type == ShaderType::BlinnPhong || type == ShaderType::PBR)
    {
        shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(transformMatrix))));
        shader.setBool("hasTangents", true);
        shader.setBool("isAnimated", false);
        shader.setBool("isTessellated", false);
    }

    // Send to GPU
    glBindVertexArray(m_VAO);
    OpenGLDebug::checkGLError("glBindVertexArray");

    glDrawArrays(GL_TRIANGLES, 0, 6);
    OpenGLDebug::checkGLError("glDrawArrays");

    glBindVertexArray(0);
    OpenGLDebug::checkGLError("glBindVertexArray");

    if (m_material && (type == ShaderType::BlinnPhong || type == ShaderType::PBR))
    {
        m_material->unbind(); // Unbind textures to prevent OpenGL state retention
        OpenGLDebug::checkGLError("Unbind");
    }
}

void engine::Billboard::clean()
{

}
