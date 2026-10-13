#include "../../include/models/mesh.h"

#include "../../include/tools/render_state.h"
#include "../../include/debug/opengl_debug.h"

#include "../../include/managers/log_manager.h"
#include "../../include/singleton.h"

#include <memory>

engine::Mesh::Mesh(const std::string& _name, std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, std::shared_ptr<Material> _material)
    : m_name(_name), m_vertices(std::move(_vertices)), m_indices(std::move(_indices)), m_material(std::move((_material)))
{
    logger.trace("Mesh constructor called");

    setupMesh();
}

// render the mesh
void engine::Mesh::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix)
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
    OpenGLDebug::checkGLError("shader.use.mesh");

    if (m_material)
    {
        if (type == ShaderType::BlinnPhong || type == ShaderType::PBR)
        {
            if (!m_material->bind(shader)) {
                std::cerr << "Failed to bind textures. Skipping draw." << std::endl;
                return;
            }

            handleOpacity(shader);

            
            if (type == ShaderType::PBR)
            {
                shader.setVec3("material.baseColorFactor", m_material->getBaseColorFactor());
                shader.setVec3("material.ambientColor", m_material->getAmbientColor());
                shader.setFloat("material.ambientIntensity", m_material->getAmbientIntensity());

                shader.setFloat("material.roughnessMapIntensity", m_material->getRougnessIntensity());
                shader.setFloat("material.metallicMapIntensity", m_material->getMetallicIntensity());
                shader.setFloat("material.ambientOcclusionMapIntensity", m_material->getAmbientOcclusionIntensity());

                shader.setFloat("material.IOR", m_material->getIOR());
                shader.setFloat("material.transmission", m_material->getTransmission());
                shader.setFloat("material.thickness", m_material->getThickness());
                shader.setVec3("material.attenuationColor", m_material->getAttenuationColor());
                shader.setFloat("material.attenuationDistance", m_material->getAttenuationDistance());
            }
            else if (type == ShaderType::BlinnPhong)
            {
                shader.setFloat("material.specularMapIntensity", m_material->getSpecularIntensity());
                shader.setFloat("material.shininess", m_material->getShininessIntensity());
                shader.setVec3("material.diffuse_color", m_material->getDiffuseColor());
                shader.setVec3("material.specular_color", m_material->getSpecularColor());
            }

            shader.setFloat("material.normalMapIntensity", m_material->getNormalIntensity());
            shader.setFloat("material.emissiveMapIntensity", m_material->getEmissiveIntensity());
            shader.setFloat("material.opacity", m_material->getOpacityIntensity());
        }
    }

    // used by all shaders (blinnphong, pbr, simpleDepthBuffer1, simpleDepthBuffer2)
    glm::mat4 finalTransform = transformMatrix;

    if (!m_isSkinned)
        finalTransform *= m_nodeTransform;

    shader.setMat4("model", finalTransform);

    if (type == ShaderType::BlinnPhong || type == ShaderType::PBR)
    {
        shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(finalTransform))));
        shader.setBool("hasTangents", m_hasTangents); // depends on loaded model and model format
    }

    // Send to GPU
    glBindVertexArray(m_VAO);
    OpenGLDebug::checkGLError("glBindVertexArray.mesh");


    bool wasFaceCullingEnabled = RenderState::isFaceCullingEnabled();

    // disable face culling for transparent primitives (to see back faces)
    if (m_material->isTransparent() || m_material->hasTransmission())
        RenderState::disableFaceCulling();

    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    OpenGLDebug::checkGLError("glDrawArrays.mesh");

    // re enable face culling for transparent primitives if face culling was enabled
    if (wasFaceCullingEnabled)
        RenderState::enableFaceCulling();
    else
        RenderState::disableFaceCulling();


    glBindVertexArray(0);
    OpenGLDebug::checkGLError("glBindVertexArray.mesh");

    // draw normals for debug
    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    SceneSettings& sceneSettings = singleton->sceneSettings();

    if (sceneSettings.drawNormalsVisualHelpers) {
        drawDebugNormals(projection, view, transformMatrix);
    }

    if (m_material && (type == ShaderType::BlinnPhong || type == ShaderType::PBR))
    {
        m_material->unbind(); // Unbind textures to prevent OpenGL state retention
        OpenGLDebug::checkGLError("Unbind.mesh");
    }

    // restore opacity defaults
    RenderState::setOpaque();
}

void engine::Mesh::setupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    // send to GPU
    glBindVertexArray(m_VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // 
    // vertex Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // vertex normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    
    // vertex texture coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);
    
    // vertex tangent
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(3);
    
    // vertex bitangent
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    glEnableVertexAttribArray(4);
    
    // bones ids
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIDs));
    glEnableVertexAttribArray(5);
    
    // bones weights
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));
    glEnableVertexAttribArray(6);

    glBindVertexArray(0);


    // During mesh setup
    m_indexCount = static_cast<unsigned int>(m_indices.size());
}

void engine::Mesh::handleOpacity(Shader& shader)
{
    bool transparent = m_material->isTransparent();
    bool cutout = m_material->isAlphaCutout();

    if (cutout)
    {
        RenderState::setOpaque();
    }
    else if (transparent)
    {
        RenderState::setTransparent();
    }
    else
    {
        RenderState::setOpaque();
    }

    if (shader.getShaderType() == ShaderType::DepthBufferDirectionalLights || shader.getShaderType() == ShaderType::DepthBufferPointLights)
    {
        // DO NOT apply transparency logic here
        RenderState::setOpaque();
    }
}

void engine::Mesh::drawDebugNormals(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix)
{
    m_debugDrawLine.init();

    glm::mat4 finalTransform = transformMatrix;

    if (!m_isSkinned)
        finalTransform *= m_nodeTransform;

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(finalTransform)));

    // vertex normal
    for (const Vertex& vertex : m_vertices)
    {
        glm::vec3 start = glm::vec3(finalTransform * glm::vec4(vertex.position, 1.0f));
        glm::vec3 normal = glm::normalize(normalMatrix * vertex.normal);
        glm::vec3 end = start + normal * DEBUG_DRAW_NORMAL_LENGTH; // Scale for visibility
        m_debugDrawLine.addLine(start, end, DEBUG_DRAW_NORMAL_COLOR, DEBUG_DRAW_NORMAL_DRAW_ARROW, DEBUG_DRAW_NORMAL_ARROW_SIZE);
    }

    // triangle normal
    /*for (size_t i = 0; i + 2 < m_indices.size(); i += 3)
    {
        const Vertex& v0 = m_vertices[m_indices[i]];
        const Vertex& v1 = m_vertices[m_indices[i + 1]];
        const Vertex& v2 = m_vertices[m_indices[i + 2]];

        glm::vec3 center =
            (v0.position + v1.position + v2.position) / 3.0f;

        glm::vec3 faceNormal =
            glm::normalize(
                glm::cross(
                    v1.position - v0.position,
                    v2.position - v0.position));

        glm::vec3 start =
            glm::vec3(finalTransform * glm::vec4(center, 1.0f));

        glm::vec3 end =
            start +
            glm::normalize(normalMatrix * faceNormal) * 0.2f;

        m_debugDrawLine.addLine(
            start,
            end,
            glm::vec3(1, 0, 0),
            true,
            0.01f);
    }*/

    m_debugDrawLine.render(view, projection);
    m_debugDrawLine.clear();
}

void engine::Mesh::clean()
{
    // mesh Data
    m_vertices.clear();
    m_indices.clear();

    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);

    m_VAO = 0;
    m_VBO = 0;
    m_EBO = 0;

    if (m_debugDrawLine.isInitialized())
        m_debugDrawLine.clean();
}

engine::Mesh::~Mesh()
{
    logger.trace("Mesh destructor called");
}