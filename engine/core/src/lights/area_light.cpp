#include "../../include/lights/area_light.h"

#include "../../include/singleton.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>  // For glm::rotation and glm::eulerAngles

#include <format>
#include <random>
#include <chrono>


engine::AreaLight::AreaLight(unsigned int index) : Light(glm::vec3(), index)
{
}

engine::AreaLight::AreaLight(glm::vec3 _position, unsigned int index) : Light(_position, index)
{
    setup();
}

void engine::AreaLight::setup()
{
    // load light cube debug shader
    //m_lightDebugShader.init("light_cube", "shaders/debug/debug_light.vertex", "shaders/debug/debug_light.frag");


    //auto matDebugLight = std::make_shared<engine::Material>(engine::Color(1.0f, 1.0f, 1.0f, 0.2f));
    //m_debug_cylinder.setup(matDebugLight);


    // CONFIGURE AREA LIGHT
    std::uniform_real_distribution<GLfloat> random_floats(0.0f, 1.0f);
    typedef std::chrono::high_resolution_clock myclock;
    unsigned seed = myclock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::function<float(void)> fn = [&random_floats, &generator] { return random_floats(generator); };

    float x = fn(); x = (x > 0.5f) ? x : -x;
    float z = fn(); z = (z > 0.5f) ? z : -z;
    offset = glm::vec3(x, 0.0f, z) * 8.f;
    yRotation = fn() * glm::two_pi<float>();
    color = glm::vec3(fn(), fn(), fn());


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


    // SHADERS
    shaderLightPlane.init("light_plane", "shaders/test/light_plane.vertex", "shaders/test/light_plane.frag");





    
}

void engine::AreaLight::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, const Color& diffuse, const Color& specular, float intensity, const glm::vec3& target, const glm::mat4 transformMatrix)
{
    std::string base = std::format("areaLights[{}]", m_index);

    // directional light
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
    shader.setFloat(str_int.c_str(), 2.0f);
    shader.setInt(str_two.c_str(), 1);

    shader.setInt("LTC1", 0);
    shader.setInt("LTC2", 1);
    shader.setInt("material.texture_diffuse", 0); // ??????????????????
    //incrementRoughness(0.0f);
    //incrementLightIntensity(0.0f);
    //switchTwoSided(false);
    //glUseProgram(0);

    shaderLightPlane.use();
    {
        glm::mat4 model(1.0f);
        shaderLightPlane.setMat4("model", model);
    }

    shaderLightPlane.setVec3("lightColor", glm::vec3(1.0f, 0.5f, 0.0f));
    glUseProgram(0);
}

void engine::AreaLight::clean()
{
}