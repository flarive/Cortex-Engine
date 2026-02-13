#pragma once

#include "../misc/noncopyable.h"
#include "../materials/material.h"
#include "../texture.h"
#include "mesh.h"
#include "../shader.h"
#include "../transform.h"
#include "bone.h"

#include <assimp/importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include "SOIL2.h"

#include <omp.h> // Include OpenMP header

#include <format>
#include <chrono>
#include <future>
#include <map>
#include <string>
#include <vector>
#include <mutex>

#include "../misc/ordered_map.h"


#include <unordered_map>
#include <functional>



namespace engine
{
    enum class ModelType { undefined = 0, model = 1, sharedModel = 2 };

    const std::unordered_map<ModelType, std::string> ModelTypeNames = {
        {ModelType::undefined, "undefined"},
        {ModelType::model, "Model"},
        {ModelType::sharedModel, "Shared model"}
    };

    inline std::string to_string(ModelType type) {
        auto it = ModelTypeNames.find(type);
        return it != ModelTypeNames.end() ? it->second : "unknown";
    }
    
    class SharedModel : private NonCopyable
    {
    public:
        // model data (TODO : make private with getters)
        std::vector<Texture> textures_loaded{};	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        std::vector<Mesh> meshes{};
        
        bool gammaCorrection{};
        bool flipUV{};

        bool highlight{};



        // at least one virtual method to make it base class
        virtual ~SharedModel() = default;

        SharedModel(bool _gamma, bool _flipUV);

        // constructor, expects a filepath to a 3D model.
        SharedModel(const std::string& _path, bool _gamma = false, bool _flipUV = false);

        SharedModel(const std::string& _path, const std::shared_ptr<Material>& _material, bool _gamma, bool _flipUV);

        ordered_map<std::string, EditorProperty> getPublicProperties() {
            return {
                {"file", EditorProperty { "File", getFilename(), readonly, 0.0f, 0.0f, 0.0f, "%s"}},
                {"meshCount", EditorProperty { "Meshes count", getMeshCount(), readonly, 0.0f, 0.0f, 0.0f, "%u" }},
                {"vertexCount", EditorProperty { "Vertex count", getVertexCount(), readonly, 0.0f, 0.0f, 0.0f, "%u" }},
                {"bonesCount", EditorProperty { "Bones count", getBoneCount(), readonly, 0.0f, 0.0f, 0.0f, "%i" }},
                {"flipUV", EditorProperty { "Flip UV", getFlipUV(), readonly, 0.0f, 1.0f, 1.0f, "%s" }},
            };
        }

        std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() {
            return {
            };
        }


        ModelType getTypeID() const
        {
            return ModelType::sharedModel;
        }

        auto& getBoneInfoMap() { return m_boneInfoMap; }
        int& getBoneCount() { return m_boneCounter; }


        bool& hasBones() { return m_hasBones; }


        bool& getFlipUV() { return flipUV; }
        void setFlipUV(bool _flipUV) { flipUV = _flipUV; }



        void setVertexBoneDataToDefault(Vertex& vertex);
        void setVertexBoneData(Vertex& vertex, int boneID, float weight);
        void extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

        unsigned int getMeshCount() const;
        unsigned int getVertexCount() const;

        std::string getFilename() const { return m_filename; }

        void reSetup();


    private:

        
        std::string m_directory{};
        std::string m_filename{};

        // processes a node in a recursive fashion.
        // Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
        void processNode(aiNode* node, const aiScene* scene);

        virtual Mesh processMesh(aiMesh* mesh, const aiScene* scene);

        bool checkMetalnessRoughnessSingleTexture(const aiScene* scene, aiMaterial* mat);

        



    protected:



        unsigned int m_numberOfMeshes{};
        unsigned int m_numberOfVertices{};

        bool m_hasBones{};
        std::map<std::string, BoneInfo> m_boneInfoMap{};
        int m_boneCounter{};

        mutable std::mutex textureMutex;

        std::shared_ptr<Material> m_customMaterial{};

        // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
        void loadModel(const std::string& path, bool flipUVs = false);

        // checks all material textures of a given type and loads the textures if they're not loaded yet.
        // the required info is returned as a Texture struct.
        std::vector<Texture> loadMaterialTextures(const aiScene* scene, aiMaterial* mat, aiTextureType type, const std::string& typeName);
    };
}