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

#include "../managers/filesystem_manager.h"

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
        SharedModel(bool _loadAnimation, bool _gamma, bool _flipUV);
        SharedModel(const std::string& _path, bool _loadAnimation, bool _gamma = false, bool _flipUV = false);
        SharedModel(const std::string& _path, const std::shared_ptr<Material>& _material, bool _loadAnimation, bool _gamma, bool _flipUV);

        // at least one virtual method to make it base class
        virtual ~SharedModel();

        ordered_map<std::string, EditorProperty> getPublicProperties() {
            return {
                {"file", EditorProperty { "File", m_fileName, readonly, 0.0f, 0.0f, 0.0f, "%s"}},
                {"meshCount", EditorProperty { "Meshes count", getMeshCount(), readonly, 0.0f, 0.0f, 0.0f, "%u" }},
                {"vertexCount", EditorProperty { "Vertex count", getVertexCount(), readonly, 0.0f, 0.0f, 0.0f, "%u" }},
                {"bonesCount", EditorProperty { "Bones count", getBoneCount(), readonly, 0.0f, 0.0f, 0.0f, "%i" }},
                {"flipUV", EditorProperty { "Flip UV", getFlipUV(), readonly, 0.0f, 1.0f, 1.0f, "%s" }},
                {"hasAnimations", EditorProperty { "Has Animations", hasAnimations(), readonly, 0.0f, 1.0f, 1.0f, "%s" }},
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


        bool hasBones();
		bool hasAnimations();
        int getBoneCount();
        std::map<std::string, BoneInfo>& getBoneInfoMap(); // return reference to avoid recopy !!!!
        

        bool& getFlipUV() { return m_flipUV; }
        void setFlipUV(bool _flipUV) { m_flipUV = _flipUV; }

        unsigned int getMeshCount() const;
        unsigned int getVertexCount() const;

        std::string getFilePath() const { return m_filePath; }
        std::string getFileName() const { return m_fileName; }

        void reSetup();

        std::vector<std::shared_ptr<Material>>& getMaterials();
        std::vector<std::shared_ptr<Mesh>>& getMeshes();
        

    private:
        std::string m_filePath{};
        std::string m_fileName{};

        
    protected:
        std::unique_ptr<MeshLoader> m_meshLoader{};


        bool m_gammaCorrection{};
        bool m_flipUV{};

        bool m_highlight{};


        std::shared_ptr<Material> m_customMaterial{};

        void loadModel(const std::string& path, bool loadAnimation = true, bool flipUVs = false);
    };
}