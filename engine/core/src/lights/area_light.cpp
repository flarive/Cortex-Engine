#include "../../include/lights/area_light.h"

#include "../../include/singleton.h"
#include "../../include/misc/colors.h"



#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>  // For glm::rotation and glm::eulerAngles


#include <format>


engine::AreaLight::AreaLight() : AreaLight(glm::vec3())
{
}

engine::AreaLight::AreaLight(glm::vec3 _position) : Light(_position)
{
    setup();
}

engine::AreaLight::AreaLight(const std::shared_ptr<engine::Primitive>& primitive, glm::vec3 _position) : m_primitive(primitive), Light(_position)
{
    setup();
}

void engine::AreaLight::setup()
{
    shaderLightPlane.init("light_plane", "shaders/test/light_plane.vert", "shaders/test/light_plane.frag");


    // Check for OpenGL errors
    //GLenum err;
    //while ((err = glGetError()) != GL_NO_ERROR) {
    //    std::cerr << "OpenGL error after loading LTC1: " << err << std::endl;
    //}


    //if (m_primitive)
    //    m_primitive->setup();

    // Send to GPU
    glGenVertexArrays(1, &areaLightVAO);
    glBindVertexArray(areaLightVAO);

    glGenBuffers(1, &areaLightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, areaLightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(areaLightVertices), areaLightVertices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // texcoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    glBindVertexArray(0);
}

void engine::AreaLight::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, const Color& diffuse, const Color& specular, float intensity, const glm::vec3& target, const glm::mat4 transformMatrix, Transform& localTransform)
{
    std::string base = std::format("areaLights[{}]", m_index);

    shader.use();
    shader.setBool(std::format("{}.use", base), m_enabled);

    // Calculate the light's points
    glm::vec3 p0 = glm::vec3(transformMatrix * glm::vec4(areaLightVertices[0].position, 1.0f));
    glm::vec3 p1 = glm::vec3(transformMatrix * glm::vec4(areaLightVertices[1].position, 1.0f));
    glm::vec3 p2 = glm::vec3(transformMatrix * glm::vec4(areaLightVertices[4].position, 1.0f));
    glm::vec3 p3 = glm::vec3(transformMatrix * glm::vec4(areaLightVertices[5].position, 1.0f));

   


    // Send the light's points to the shader
    std::string str_pos = std::format("{}.points", base);
    std::string str_col = std::format("{}.color", base);
    std::string str_int = std::format("{}.intensity", base);
    std::string str_two = std::format("{}.twoSided", base);

    shader.setVec3((str_pos + "[0]").c_str(), p0);
    shader.setVec3((str_pos + "[1]").c_str(), p1);
    shader.setVec3((str_pos + "[2]").c_str(), p2);
    shader.setVec3((str_pos + "[3]").c_str(), p3);


    shader.setVec3(str_col.c_str(), color);
    shader.setFloat(str_int.c_str(), intensity);
    shader.setInt(str_two.c_str(), twoSided ? 1 : 0);


    glm::vec3 temp = Colors::SlateGray;
    shader.setVec4("material.albedoRoughness", glm::vec4(temp, roughness));

    shaderLightPlane.use();
    shaderLightPlane.setMat4("model", transformMatrix);
    shaderLightPlane.setMat4("view", view);
    shaderLightPlane.setMat4("projection", projection);
    shaderLightPlane.setVec3("lightColor", color);

	//m_primitive->draw(shader, transformMatrix, localTransform);
        
    // send to GPU
    glBindVertexArray(areaLightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);


    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    SceneSettings& sceneSettings = singleton->sceneSettings();

    if (sceneSettings.drawNormalsVisualHelpers) {
        drawDebugNormals(p0, p1, p2, p3, projection, view, transformMatrix);
    }
}


void engine::AreaLight::drawDebugNormals(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix)
{
    m_debugDrawLine.init();

    glm::vec3 center = 0.25f * (p0 + p1 + p2 + p3);
    glm::vec3 normal = glm::normalize(glm::cross(p1 - p0, p3 - p0));
    glm::vec3 end = center + normal * 0.5f;

    // draw a debug line from center -> end
    m_debugDrawLine.addLine(center, end, glm::vec3(0.0f, 1.0f, 0.0f), true, 0.06f);

    m_debugDrawLine.render(view, projection);
    m_debugDrawLine.clear();
}

void engine::AreaLight::clean()
{
}