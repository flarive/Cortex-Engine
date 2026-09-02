#include "../../include/models/model.h"
#include "../../include/models/shared_model.h"

#include "../../include/textures/texture.h"

#include "../../include/managers/log_manager.h"
#include "../../include/tools/helpers.h"

#include <omp.h> // Include OpenMP header

#include <format>
#include <chrono>
#include <future>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>  // For glm::rotation and glm::eulerAngles


// constructor, expects a filepath to a 3D model.
engine::Model::Model(const std::string& _path, bool _loadAnimation, bool _gamma, bool _flipUVs, const glm::vec3& _position)
    : SharedModel(_path, _loadAnimation, _gamma, _flipUVs), m_position(_position)
{
    logger.trace("Model constructor called");
}

engine::Model::Model(const std::string& _path, const std::shared_ptr<Material>& _material, bool _loadAnimation, bool _gamma, bool _flipUVs, const glm::vec3& _position)
    : SharedModel(_path, _material, _loadAnimation, _gamma, _flipUVs), m_position(_position)
{
    logger.trace("Model constructor called");
}

// constructor, expects a model (for sharing)
engine::Model::Model(const std::shared_ptr<SharedModel>& _shared_model, bool _loadAnimation, bool _gamma, bool _flipUVs, const glm::vec3& _position)
    : SharedModel(_loadAnimation, _gamma, _flipUVs), m_shared_model(_shared_model), m_position(_position)
{
    logger.trace("Model constructor called");
}

// draws the model, and thus all its meshes
void engine::Model::draw(Shader& shader, const glm::mat4& transformMatrix, Transform& localTransform)
{
    if (!m_isEnabled)
        return;

    ShaderType type = shader.getShaderType();
    
    setTransform(localTransform.getLocalPosition(), localTransform.getLocalRotation(), localTransform.getLocalScale());

    shader.use();
    shader.setBool("isAnimated", hasBones());

    
    if (type == ShaderType::BlinnPhong || type == ShaderType::PBR)
    {
        shader.setBool("isTessellated", false);
    }

    if (!m_shared_model)
    {
        for (auto& mesh : getMeshes()) {
            mesh->draw(shader, transformMatrix);
        }
    }
    else
    {
        // shared model, loaded one time, drawn multiple times
        for (auto& mesh : m_shared_model->getMeshes()) {
            mesh->draw(shader, transformMatrix);
        }
    }
}

void engine::Model::clean()
{
    if (m_meshLoader)
    {
        m_meshLoader.reset();
    }
}

unsigned int engine::Model::getMeshCount() const
{
    if (!m_shared_model)
    {
        return SharedModel::getMeshCount();
    }
    else
    {
        return m_shared_model->getMeshCount();
    }
}

engine::Model::~Model()
{
    logger.trace("Model destructor called");
}