#include "../../include/primitives/cube.h"

#include "../../include/vertex.h"
#include "../../include/uvmapping.h"
#include "../../include/materials/material.h"
#include "../../include/tools/helpers.h"

#include "../../include/singleton.h"
#include "../../include/managers/log_manager.h"

#include <vector>
#include <glm/glm.hpp>

engine::Cube::Cube(const glm::vec3& _position)
    : Primitive(_position)
{
    logger.trace("Cube constructor called");
}

engine::Cube::Cube(const float& _size, const glm::vec3& _position)
    : Primitive(_position), m_width(_size), m_height(_size), m_depth(_size)
{
    logger.trace("Cube constructor called");
}

engine::Cube::Cube(const float& _width, const float& _height, const float& _depth, const glm::vec3& _position)
    : Primitive(_position), m_width(_width), m_height(_height), m_depth(_depth)
{
    logger.trace("Cube constructor called");
}

engine::Cube::~Cube()
{
    logger.trace("Cube destructor called");
}

void engine::Cube::setup()
{
    geometrySetup(); // Geometry setup
}

void engine::Cube::setup(const std::shared_ptr<Material>& material)
{
    Primitive::setMaterial(material);

    const UvMapping uv{};
    setup(material, uv);
}

void engine::Cube::setup(const std::shared_ptr<Material>& material, const UvMapping& uv)
{
    Primitive::setMaterial(material);
    m_uvScale = uv.getUvScale();

    geometrySetup(); // Geometry setup

    if (material && material->hasDiffuseMap())
        material->loadTexturesAsync(); // Let material handle texture loading
}

void engine::Cube::geometrySetup()
{
    std::vector<Vertex> vertices = generateVertices();

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

std::vector<engine::Vertex> engine::Cube::generateVertices()
{
    return generateCuboidVertices(m_width, m_height, m_depth, m_uvScale);
}

void engine::Cube::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform)
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
    OpenGLDebug::checkGLError("shader.use33");

    setTransform(localTransform.getLocalPosition(), localTransform.getLocalRotation(), localTransform.getLocalScale());

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

        
        
        shader.setBool("material.useParallaxMapping", m_material->useParallaxMapping());
        shader.setFloat("material.parallaxMapIntensity", m_material->getParallaxIntensity());
        

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

    glDrawArrays(GL_TRIANGLES, 0, 36);
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

void engine::Cube::drawDebugNormals(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix)
{
    m_debugDrawLine.init();

    // Define the local normals for each face of the cube
    const std::vector<glm::vec3> localNormals = {
        glm::vec3(1.0f, 0.0f, 0.0f),   // +X (right)
        glm::vec3(-1.0f, 0.0f, 0.0f),  // -X (left)
        glm::vec3(0.0f, 1.0f, 0.0f),   // +Y (top)
        glm::vec3(0.0f, -1.0f, 0.0f),  // -Y (bottom)
        glm::vec3(0.0f, 0.0f, 1.0f),   // +Z (front)
        glm::vec3(0.0f, 0.0f, -1.0f)   // -Z (back)
    };

    // Define the local centers for each face of the cube (assuming cube is axis-aligned and centered at origin)
    const std::vector<glm::vec3> localFaceCenters = {
        glm::vec3(0.5f, 0.0f, 0.0f),   // +X face center
        glm::vec3(-0.5f, 0.0f, 0.0f),  // -X face center
        glm::vec3(0.0f, 0.5f, 0.0f),   // +Y face center
        glm::vec3(0.0f, -0.5f, 0.0f),  // -Y face center
        glm::vec3(0.0f, 0.0f, 0.5f),   // +Z face center
        glm::vec3(0.0f, 0.0f, -0.5f)   // -Z face center
    };

    // Transform each face center and normal to world space
    for (size_t i = 0; i < localNormals.size(); ++i) {
        // Transform the face center to world space
        glm::vec3 worldFaceCenter = glm::vec3(transformMatrix * glm::vec4(localFaceCenters[i], 1.0f));
        // Transform the normal to world space
        glm::vec3 worldNormal = glm::normalize(glm::mat3(transformMatrix) * localNormals[i]);
        // Compute the end point of the normal line
        glm::vec3 end = worldFaceCenter + worldNormal * 0.25f; // Scale for visibility
        // Add the debug line (with arrow)
        m_debugDrawLine.addLine(worldFaceCenter, end, glm::vec3(1.0f, 0.0f, 0.0f), true, 0.06f); // Red color for normal
    }

    // Render all debug lines
    m_debugDrawLine.render(view, projection);
    m_debugDrawLine.clear();
}


void engine::Cube::clean()
{
    if (m_debugDrawLine.isInitialized())
        m_debugDrawLine.clean();
}