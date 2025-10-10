#include "../../include/lights/area_light.h"

#include "../../include/singleton.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>  // For glm::rotation and glm::eulerAngles

#include <format>


engine::AreaLight::AreaLight(unsigned int index) : AreaLight(glm::vec3(), index)
{
}

engine::AreaLight::AreaLight(glm::vec3 _position, unsigned int index) : Light(_position, index)
{
    setup();
}

void engine::AreaLight::setup()
{
    // SHADERS
    shaderLightPlane.init("light_plane", "shaders/test/light_plane.vertex", "shaders/test/light_plane.frag");


    // LUT textures
    LTC_matrices mLTC;
    mLTC.mat1 = Texture::loadMTexture();
    mLTC.mat2 = Texture::loadLUTTexture();

    // SEND TO GPU
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

void engine::AreaLight::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, const Color& diffuse, const Color& specular, float intensity, const glm::vec3& target, const glm::mat4 transformMatrix)
{
    std::string base = std::format("areaLights[{}]", m_index);

    // area light
    shader.setBool(std::format("{}.use", base), true);

    // SHADER CONFIGURATION
    shader.use();

    glm::mat4 model(1.0f);
    model = glm::translate(model, offset);
    model = glm::rotate(model, yRotation, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::vec3 p0 = glm::vec3(model * glm::vec4(areaLightVertices[0].position, 1.0f));
    glm::vec3 p1 = glm::vec3(model * glm::vec4(areaLightVertices[1].position, 1.0f));
    glm::vec3 p2 = glm::vec3(model * glm::vec4(areaLightVertices[4].position, 1.0f));
    glm::vec3 p3 = glm::vec3(model * glm::vec4(areaLightVertices[5].position, 1.0f));


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

    shader.setInt("LTC1", 0);
    shader.setInt("LTC2", 1);
    //shader.setInt("material.texture_diffuse", 0); // ??????????????????
    //incrementRoughness(0.0f);
    //incrementLightIntensity(0.0f);
    //switchTwoSided(false);
    glUseProgram(0); // ??????????

    shaderLightPlane.use();
    {
        glm::mat4 model(1.0f);
        shaderLightPlane.setMat4("model", model);
    }

    shaderLightPlane.setVec3("lightColor", glm::vec3(1.0f, 0.5f, 0.0f));
    glUseProgram(0); // ??????????


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mLTC.mat1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mLTC.mat2);
    /*glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, concreteTexture);*/


    glUseProgram(0);

    // draw area light planes
    shaderLightPlane.use();
    shaderLightPlane.setMat4("view", view);
    shaderLightPlane.setMat4("projection", projection);
    
    
    
    model = glm::mat4(1.0f);
    model = glm::translate(model, offset);
    model = glm::rotate(model, yRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    shaderLightPlane.setMat4("model", model);
    shaderLightPlane.setVec3("lightColor", color);
        
    glBindVertexArray(areaLightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    
    glUseProgram(0);

    
}

void engine::AreaLight::clean()
{
}