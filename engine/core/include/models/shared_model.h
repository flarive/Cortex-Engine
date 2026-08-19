#pragma once

#include "../misc/noncopyable.h"
#include "../materials/material.h"
#include "../materials/blinnphong_material.h"
#include "../materials/pbr_material.h"
#include "../textures/texture.h"
#include "mesh.h"
#include "../shader.h"
#include "../transform.h"
#include "bone.h"

#include "mesh_loader.h"

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
        SharedModel(bool _gamma, bool _flipUV);
        SharedModel(const std::string& _path, bool _gamma = false, bool _flipUV = false);
        SharedModel(const std::string& _path, const std::shared_ptr<Material>& _material, bool _gamma, bool _flipUV);

        // at least one virtual method to make it base class
        virtual ~SharedModel();

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


        bool& getFlipUV() { return m_flipUV; }
        void setFlipUV(bool _flipUV) { m_flipUV = _flipUV; }

        unsigned int getMeshCount() const;
        unsigned int getVertexCount() const;

        std::string getFilename() const { return m_filename; }

        void reSetup();

        std::vector<std::shared_ptr<Material>>& getMaterials() { return m_materials; }
        std::vector<std::shared_ptr<Mesh>>& getMeshes();



    private:
        std::string m_directory{};
        std::string m_filename{};

        std::vector<std::shared_ptr<Material>> m_materials{};


        
    protected:
        std::unique_ptr<MeshLoader> m_meshLoader{};

        //std::vector<std::shared_ptr<Mesh>> m_meshes{};

        bool m_gammaCorrection{};
        bool m_flipUV{};

        bool m_highlight{};

        bool m_hasBones{};
        std::map<std::string, BoneInfo> m_boneInfoMap{};
        int m_boneCounter{};

        mutable std::mutex textureMutex;

        std::shared_ptr<Material> m_customMaterial{};

        // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
        void loadModel(const std::string& path, bool flipUVs = false);
    };
}