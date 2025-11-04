#include "../../include/primitives/billboard.h"

#include "../../include/vertex.h"
#include "../../include/uvmapping.h"
#include "../../include/tools/helpers.h"

engine::Billboard::Billboard(bool _flipNormals, const glm::vec3& _position) : m_flipNormals(_flipNormals), Primitive(_position)
{
}

void engine::Billboard::setup()
{
    geometrySetup(); // Geometry setup
}

void engine::Billboard::setup(const std::shared_ptr<Material>& material)
{
    m_material = material; // Store material reference

    const UvMapping uv{};
    setup(material, uv);
}

void engine::Billboard::setup(const std::shared_ptr<Material>& material, const UvMapping& uv)
{
    m_material = material;
    m_uvScale = uv.getUvScale();

    geometrySetup(); // Geometry setup

    if (material && material->hasDiffuseMap())
        material->loadTexturesAsync(); // Let material handle texture loading
}

void engine::Billboard::geometrySetup()
{
    glGenVertexArrays(1, &m_VAO);  // 1 is the uniqueID of the VAO
    glGenBuffers(1, &m_VBO);  // 1 is the uniqueID of the VBO

    // Send to GPU
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(m_VAO);

    float* quadVertices = engine::Primitive::GetScaledQuadVertices(1.0f);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, 48 * sizeof(float), quadVertices, GL_STATIC_DRAW);

    GLsizei stride = 8;

    // position attribute (XYZ)
    // layout (location = 0), vec3, vector of floats, normalized, stride, offset in buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // stride 0 to 2


    // normal attribute (XYZ)
    // layout(location = 1), vec3, vector of floats, normalized, stride, offset in buffer
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); // stride 3 to 5

    // texture coord attribute (RGB)
    // layout(location = 2), vec3, vector of floats, normalized, stride, offset in buffer
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2); // stride 6 to 7
}


std::vector<engine::Vertex> engine::Billboard::generateVertices()
{
    return generateBillboardVertices2(m_uvScale, m_flipNormals);
}

void engine::Billboard::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform)
{
    if (!m_material || !shader.isValid()) {
        std::cerr << "Material or shader not valid. Skipping draw." << std::endl;
        return;
    }

    if (!m_material->getAllTexturesLoaded()) {
        std::cout << "Textures not ready. Deferring draw." << std::endl;
        return;
    }

    if (m_VAO == 0 || m_VBO == 0) {
        std::cerr << "VAO/VBO not initialized. Skipping draw." << std::endl;
        return;
    }

    shader.use();
    OpenGLDebug::checkGLError("shader.use");

    position = localTransform.getLocalPosition();
    rotation = localTransform.getLocalRotation();
    scale = localTransform.getLocalScale();

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
        }
    }

    glEnable(GL_BLEND);
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

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

    glDrawArrays(GL_TRIANGLES, 0, 6);
    OpenGLDebug::checkGLError("glDrawArrays");

    glBindVertexArray(0);
    OpenGLDebug::checkGLError("glBindVertexArray");

    m_material->unbind(); // Unbind textures to prevent OpenGL state retention
    OpenGLDebug::checkGLError("Unbind");
}

//void engine::Billboard::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform)
//{
//    shader.use();
//
//    position = localTransform.getLocalPosition();
//    scale = localTransform.getLocalScale();
//
//    // Extract camera position from view matrix
//    glm::vec3 cameraRight = glm::vec3(view[0][0], view[1][0], view[2][0]);
//    glm::vec3 cameraUp = glm::vec3(view[0][1], view[1][1], view[2][1]);
//
//    // Build billboard rotation matrix
//    glm::mat4 billboardRotation = glm::mat4(1.0f);
//    billboardRotation[0] = glm::vec4(cameraRight, 0.0f);
//    billboardRotation[1] = glm::vec4(cameraUp, 0.0f);
//    billboardRotation[2] = glm::vec4(glm::normalize(glm::cross(cameraRight, cameraUp)), 0.0f);
//
//    // Build model matrix: translation * rotation * scale
//    glm::mat4 model = glm::translate(glm::mat4(1.0f), position)
//        * billboardRotation
//        * glm::scale(glm::mat4(1.0f), scale);
//
//    shader.setMat4("model", model);
//    shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
//    shader.setBool("hasTangents", true);
//
//    if (m_material)
//    {
//        m_material->bind(shader);
//        shader.setVec3("material.ambient_color", m_material->getAmbientColor());
//        shader.setVec3("material.diffuse_color", m_material->getDiffuseColor());
//        shader.setVec3("material.specular_color", m_material->getSpecularColor());
//        shader.setFloat("material.shininess", m_material->getShininessIntensity());
//        shader.setFloat("material.ambient_intensity", m_material->getAmbientIntensity());
//    }
//
//    glEnable(GL_BLEND);
//
//    // Send to GPU
//    glBindVertexArray(m_VAO);
//    glDrawArrays(GL_TRIANGLES, 0, 6);
//    glBindVertexArray(0);
//
//    m_material->unbind();
//}

void engine::Billboard::clean()
{

}
