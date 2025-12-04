#include "../../include/models/model.h"
#include "../../include/models/shared_model.h"

#include "../../include/texture.h"

#include "../../include/managers/log_manager.h"
#include "../../include/tools/file_system.h"
#include "../../include/tools/helpers.h"

#include "SOIL2.h"

#include <omp.h> // Include OpenMP header

#include <format>
#include <chrono>
#include <future>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>  // For glm::rotation and glm::eulerAngles


// constructor, expects a filepath to a 3D model.
engine::Model::Model(const std::string& _path, bool _hasBones, bool _gamma, bool _flipUVs, const glm::vec3& _position)
    : SharedModel(_path, _hasBones, _gamma, _flipUVs), position(_position)
{
}

// constructor, expects a model (for sharing)
engine::Model::Model(const std::shared_ptr<SharedModel>& _shared_model, bool _hasBones, bool _gamma, bool _flipUVs, const glm::vec3& _position)
    : SharedModel(_hasBones, _gamma, _flipUVs), m_shared_model(_shared_model), position(_position)
{
}

// draws the model, and thus all its meshes
void engine::Model::draw(Shader& shader, const glm::mat4& transformMatrix, Transform& localTransform)
{
    position = localTransform.getLocalPosition();
    rotation = localTransform.getLocalRotation();
    scale = localTransform.getLocalScale();

    bool aaa = hasBones();
    shader.setBool("isAnimated", hasBones());

    if (!m_shared_model)
    {
        for (auto& mesh : meshes) {
            mesh.draw(shader, transformMatrix);
        }
    }
    else
    {
        // shared model, loaded one time, drawn multiple times
        for (auto& mesh : m_shared_model->meshes) {
            mesh.draw(shader, transformMatrix);
        }
    }
}

void engine::Model::clean()
{
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].clean();
    }

    textures_loaded.clear();
    meshes.clear();
}

unsigned int engine::Model::getNumberOfMeshes() const
{
    if (!m_shared_model)
    {
        return m_numberOfMeshes;
    }
    else
    {
        return m_shared_model->getNumberOfMeshes();
    }
}