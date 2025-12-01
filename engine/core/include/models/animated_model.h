#pragma once

#include "../misc/noncopyable.h"
#include "../texture.h"
#include "mesh.h"
#include "../shader.h"
#include "../transform.h"
#include "model.h"

#include <glm/glm.hpp>

#include <assimp/importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "assimp_glm_helpers.h"
#include "animdata.h"



#include <string>
#include <vector>
#include <map>
#include <mutex>

namespace engine
{
    class AnimatedModel : public Model
    {
    public:
        glm::vec3 position{};
        glm::vec3 rotation{};
        glm::vec3 scale{};

        AnimatedModel() = default;
        ~AnimatedModel() = default;

        // constructor, expects a filepath to a 3D model.
        AnimatedModel(const std::string& _path, bool _gamma = false, bool _flipUVs = false, const glm::vec3& _position = glm::vec3());

        //AnimatedModel(const std::shared_ptr<SharedModel>& _shared_model, bool _gamma = false, bool _flipUVs = false, const glm::vec3& _position = glm::vec3());


        auto& getBoneInfoMap() { return m_BoneInfoMap; }
        int& getBoneCount() { return m_BoneCounter; }

        void setVertexBoneDataToDefault(Vertex& vertex);
        void setVertexBoneData(Vertex& vertex, int boneID, float weight);
        void extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

        Mesh processMesh(aiMesh* mesh, const aiScene* scene) override;

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::mat4& transformMatrix, Transform& localTransform);
        void draw(Shader& shader);

        void clean();

        unsigned int getNumberOfMeshes() const;


    private:
        std::map<std::string, BoneInfo> m_BoneInfoMap{};
        int m_BoneCounter{0};

        // for shared model only (loaded one time, drawn multiple times)
        std::shared_ptr<SharedModel> m_shared_model{};
    };
}
