#pragma once

#include "../misc/noncopyable.h"
#include "../texture.h"
#include "mesh.h"
#include "../shader.h"
#include "../transform.h"
#include "shared_model.h"

#include <glm/glm.hpp>

#include <assimp/importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include <string>
#include <vector>
#include <mutex>

namespace engine
{
    class Model : public SharedModel
    {
    public:
        glm::vec3 position{};
        glm::vec3 rotation{};
        glm::vec3 scale{};

        Model() = default;
		~Model() = default;

        // constructor, expects a filepath to a 3D model.
        Model(const std::string& _path, bool _hasBones = false, bool _gamma = false, bool _flipUVs = false, const glm::vec3& _position = glm::vec3());

        Model(const std::shared_ptr<SharedModel>& _shared_model, bool _hasBones = false, bool _gamma = false, bool _flipUVs = false, const glm::vec3& _position = glm::vec3());

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::mat4& transformMatrix, Transform& localTransform);

        void clean();

        unsigned int getNumberOfMeshes() const;

    private:
        // for shared model only (loaded one time, drawn multiple times)
        std::shared_ptr<SharedModel> m_shared_model{};
    };
}
