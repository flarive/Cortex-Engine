#include "../../include/primitives/billboard.h"

#include "../../include/vertex.h"
#include "../../include/uvmapping.h"
#include "../../include/tools/helpers.h"

engine::Billboard::Billboard(const glm::vec3& _position) : Primitive(_position)
{
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

    setup(); // Geometry setup

    if (material && material->hasDiffuseMap())
        material->loadTexturesAsync(); // Let material handle texture loading
}

void engine::Billboard::setup()
{
    glGenVertexArrays(1, &m_VAO);  // 1 is the uniqueID of the VAO
    glGenBuffers(1, &m_VBO);  // 1 is the uniqueID of the VBO

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
    return generateBillboardVertices(m_uvScale);
}


// draws the model, and thus all its meshes
//void engine::Billboard::draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation)
//{
//    shader.use();
//
//    if (m_material)
//    {
//        m_material->bind(shader);
//        shader.setVec3("material.ambient_color", m_material->getAmbientColor());
//        shader.setVec3("material.diffuse_color", m_material->getDiffuseColor());
//        shader.setVec3("material.specular_color", m_material->getSpecularColor());
//
//        shader.setFloat("material.shininess", m_material->getShininessIntensity());
//
//        shader.setFloat("material.ambient_intensity", m_material->getAmbientIntensity());
//    }
//
//    glEnable(GL_BLEND);
//    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
//
//
//    auto normalizedRotation = engine::Helpers::normalizeRotation(rotation);
//
//
//    // calculate the model matrix for each object and pass it to shader before drawing
//    glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
//    model = glm::translate(model, position);
//    if (normalizedRotation.angle != 0) model = glm::rotate(model, glm::radians(normalizedRotation.angle), normalizedRotation.axis);
//    model = glm::scale(model, size);
//    shader.setMat4("model", model);
//    shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
//    shader.setBool("hasTangents", true);
//
//    // Render billboard
//    glBindVertexArray(m_VAO);
//    glDrawArrays(GL_TRIANGLES, 0, 6);
//
//    glBindVertexArray(0);
//
//    m_material->unbind(); // Unbind textures to prevent OpenGL state retention
//}

void engine::Billboard::draw(Shader& shader, const glm::mat4 transformMatrix, Transform& localTransform)
{
    shader.use();

    //position = localTransform.getLocalPosition();
    rotation = localTransform.getLocalRotation();
    scale = localTransform.getLocalScale();

    if (m_material)
    {
        m_material->bind(shader);
        shader.setVec3("material.ambient_color", m_material->getAmbientColor());
        shader.setVec3("material.diffuse_color", m_material->getDiffuseColor());
        shader.setVec3("material.specular_color", m_material->getSpecularColor());

        shader.setFloat("material.shininess", m_material->getShininessIntensity());

        shader.setFloat("material.ambient_intensity", m_material->getAmbientIntensity());
    }

    glEnable(GL_BLEND);
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);


  
    shader.setMat4("model", transformMatrix);
    shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(transformMatrix))));
    shader.setBool("hasTangents", true);

    // Render billboard
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);

    m_material->unbind(); // Unbind textures to prevent OpenGL state retention
}
