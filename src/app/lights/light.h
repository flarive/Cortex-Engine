#pragma once

#include "../common_defines.h"
#include "../shader.h"
#include "../primitive.h"

//#define LIGHT_CUBE_SIZE 0.02f;

/// <summary>
/// Abstract class for lights
/// </summary>
class Light
{
public:
    Light() : m_index(0) {}
    Light(unsigned int index) : m_index(index) {}
    virtual ~Light() = default;

    virtual void draw(const Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::vec3& position, const glm::vec3& target = glm::vec3(0,0,0)) = 0;
    virtual void clean() = 0;


private:
    

 protected:
    // render data 
    unsigned int VBO = 0, VAO = 0;

    unsigned int m_index = 0;

    Shader lightCubeShader;

    const float LIGHT_CUBE_SIZE = 0.02f;
};
