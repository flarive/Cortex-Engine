#include "../include/mesh.h"

#include "../include/debug/opengl_debug.h"

#include <memory>

engine::Mesh::Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, std::shared_ptr<Material> _material)
    : vertices(std::move(_vertices)), indices(std::move(_indices)), m_material(std::move((_material)))
{
    setupMesh();
}

// render the mesh
void engine::Mesh::draw(Shader& shader, const glm::mat4 transformMatrix)
{
    assert(m_material);

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
    OpenGLDebug::checkGLError("shader.use");

    if (m_material)
    {
        if (shader.name == "blinnphong" || shader.name == "pbr")
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

            shader.setFloat("material.heightScale", m_material->getHeightIntensity());
            shader.setFloat("material.normalMapIntensity", m_material->getNormalIntensity());
            shader.setFloat("material.emissiveIntensity", m_material->getEmissiveIntensity());
        }
    }

    // used by all shaders (blinnphong, pbr, simpleDepthBuffer1, simpleDepthBuffer2)
    shader.setMat4("model", transformMatrix);

    if (shader.name == "blinnphong" || shader.name == "pbr")
    {
        shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(transformMatrix))));
        shader.setBool("hasTangents", true);
    }


    // Send to GPU
    glBindVertexArray(m_VAO);
    OpenGLDebug::checkGLError("glBindVertexArray");

    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    OpenGLDebug::checkGLError("glDrawArrays");

    glBindVertexArray(0);
    OpenGLDebug::checkGLError("glBindVertexArray");

    if (m_material && (shader.name == "blinnphong" || shader.name == "pbr"))
    {
        m_material->unbind(); // Unbind textures to prevent OpenGL state retention
        OpenGLDebug::checkGLError("Unbind");
    }
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
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

    glBindVertexArray(0);


    // During mesh setup
    m_indexCount = static_cast<unsigned int>(indices.size());
}

void engine::Mesh::clean()
{
    // mesh Data
    vertices.clear();
    indices.clear();

    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);

    m_VAO = 0;
    m_VBO = 0;
    m_EBO = 0;
}