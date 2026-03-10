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
    
    // Bones ids (not really needed here)
    //glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIDs));
    //glEnableVertexAttribArray(5);

    //// Bones weights (not really needed here)
    //glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));
    //glEnableVertexAttribArray(6);

    glBindVertexArray(0);
}

std::vector<engine::Vertex> engine::Plane::generateVertices()
{
    return Primitive::generatePlaneVertices(m_uvScale, m_flipNormals);
}

void engine::Plane::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform)
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
    OpenGLDebug::checkGLError("shader.use11");

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

            


            //shader.setFloat("material.heightScale", m_material->getHeightIntensity());
            shader.setFloat("material.normalMapIntensity", m_material->getNormalIntensity());
            

            shader.setBool("material.canCastShadows", canCastShadows());
            shader.setBool("material.canReceiveShadows", canReceiveShadows());

            if (type == ShaderType::PBR)
            {
                shader.setVec3("material.ambient_color", m_material->getAmbientColor());
                shader.setFloat("material.ambient_intensity", m_material->getAmbientIntensity());
                shader.setFloat("material.emissiveIntensity", m_material->getEmissiveIntensity());
            }
        }
    }

    // used by all shaders (blinnphong, pbr, simpleDepthBuffer1, simpleDepthBuffer2)
    shader.setMat4("model", transformMatrix);


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


    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    SceneSettings& sceneSettings = singleton->sceneSettings();

    if (sceneSettings.drawNormalsVisualHelpers) {
        drawDebugNormals(projection, view, transformMatrix);
    }

    if (m_material && (type == ShaderType::BlinnPhong || type == ShaderType::PBR))
    {
        m_material->unbind(); // Unbind textures to prevent OpenGL state retention
        OpenGLDebug::checkGLError("Unbind");
    }
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
    if (m_debugDrawLine.isInitialized())
        m_debugDrawLine.clean();
}
