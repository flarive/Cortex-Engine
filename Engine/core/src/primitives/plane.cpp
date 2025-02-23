#include "../../include/primitives/plane.h"

#include "../../include/texture.h"
#include "../../include/uvmapping.h"
#include "../../include/primitives/primitive.h"

//void engine::Plane::setup(const glm::uvec3& color)
//{
//    m_diffuseMap = engine::Texture::createSolidColorTexture(color.r, color.g, color.b, 255);
//}
//
//void engine::Plane::setup(const engine::Material& material)
//{
//    const UvMapping uv{};
//    setup(material, uv);
//}
//
//void engine::Plane::setup(const engine::Material& material, const UvMapping& uv)
//{
//    m_uvScale = uv.getUvScale();
//    
//    setup();
//
//    m_ambientColor = material.getAmbientColor();
//
//    // load textures
//    if (material.hasDiffuseMap())
//        m_diffuseMap = engine::Texture::loadTexture(material.getDiffuseTexPath(), true, false);
//
//    if (material.hasSpecularMap())
//        m_specularMap = engine::Texture::loadTexture(material.getSpecularTexPath(), true, false);
//
//    if (material.hasNormalMap())
//        m_normalMap = engine::Texture::loadTexture(material.getNormalTexPath(), true, false);
//}

void engine::Plane::setup(const std::shared_ptr<engine::Material2>& material)
{
    m_material = material; // Store material reference

    const UvMapping uv{};
    setup(material, uv);
}

void engine::Plane::setup(const std::shared_ptr<engine::Material2>& material, const UvMapping& uv)
{
    m_material = material;
    m_uvScale = uv.getUvScale();

    setup(); // Geometry setup

    if (material)
        material->loadTextures(); // Let material handle texture loading
}

void engine::Plane::setup()
{
    glGenVertexArrays(1, &m_VAO);  // 1 is the uniqueID of the VAO
    glGenBuffers(1, &m_VBO);  // 1 is the uniqueID of the VBO

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(m_VAO);

    float* planeVertices = engine::Primitive::getScaledPlaneVertices(m_uvScale);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, 48 * sizeof(float), planeVertices, GL_STATIC_DRAW);

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


    delete[] planeVertices;
}

// draws the model, and thus all its meshes
void engine::Plane::draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, float rotationAngle, const glm::vec3& rotationAxis)
{
    //// bind diffuse (albedo) map
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, m_diffuseMap);

    //// bind specular map
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, m_specularMap);

    //// bind normal map
    //glActiveTexture(GL_TEXTURE2);
    //glBindTexture(GL_TEXTURE_2D, m_normalMap);

    //// bind metallic map
    //glActiveTexture(GL_TEXTURE3);
    //glBindTexture(GL_TEXTURE_2D, m_metallicMap);

    //// bind roughness map
    //glActiveTexture(GL_TEXTURE4);
    //glBindTexture(GL_TEXTURE_2D, m_roughnessMap);

    //// bind ambient occlusion map
    //glActiveTexture(GL_TEXTURE5);
    //glBindTexture(GL_TEXTURE_2D, m_aoMap);

    //// bind height map
    //glActiveTexture(GL_TEXTURE6);
    //glBindTexture(GL_TEXTURE_2D, m_heightMap);
    //
    //if (shader.name == "pbr")
    //{
    //}
    //else if (shader.name == "blinnphong") // blinn phong shader
    //{
    //    shader.use();
    //    shader.setVec3("material.ambient_color", 0.01f, 0.01f, 0.01f);
    //    //shader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
    //    shader.setInt("material.texture_diffuse1", 0); // texture 0
    //    shader.setInt("material.texture_specular1", 1); // texture 1
    //    shader.setInt("material.texture_normal1", 2); // texture 2
    //    //shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    //    shader.setBool("material.has_texture_normal_map", m_normalMap != 0);
    //    shader.setFloat("uvScale", 2.0f);
    //}
    //

    //// render the cubes
    //glBindVertexArray(m_VAO);

    //// calculate the model matrix for each object and pass it to shader before drawing
    //glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    //model = glm::translate(model, position);
    //if (rotationAngle != 0) model = glm::rotate(model, glm::radians(rotationAngle), rotationAxis);
    //model = glm::scale(model, glm::vec3(size.x, 0.01f, size.z));
    //shader.setMat4("model", model);

    //glDrawArrays(GL_TRIANGLES, 0, 6);

    //glBindVertexArray(0);


    shader.use();

    if (m_material)
    {
        m_material->bind(shader, false);
        shader.setVec3("material.ambient_color", m_material->getAmbientColor());
        shader.setFloat("uvScale", m_uvScale);
    }

    glBindVertexArray(m_VAO);

    // calculate the model matrix for each object and pass it to shader before drawing
    glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    model = glm::translate(model, position);
    if (rotationAngle != 0) model = glm::rotate(model, glm::radians(rotationAngle), rotationAxis);
    model = glm::scale(model, glm::vec3(size.x, 0.01f, size.z));
    shader.setMat4("model", model);

    // Render plane
    
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);

    m_material->unbind(); // Unbind textures to prevent OpenGL state retention
}

void engine::Plane::clean()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);

    m_VAO = 0;
    m_VBO = 0;
    m_EBO = 0;
}