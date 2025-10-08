#pragma once

#include "misc/noncopyable.h"
#include "texture.h"
#include "mesh.h"
#include "shader.h"
#include "transform.h"


#include <assimp/importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



#include <string>
#include <vector>
#include <mutex>

namespace engine
{
    // just to have std::dynamic_pointer_cast working on Model class
    class SharedModel : private NonCopyable {
    public:
		// model data (TODO : make private with getters)
        std::vector<Texture> textures_loaded{};	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        std::vector<Mesh> meshes{};
        std::string directory{};
        bool gammaCorrection{};

        
        
        
		// at least one virtual method to make it base class
        virtual ~SharedModel() = default;

        SharedModel(bool gamma, bool flipUVs);

        // constructor, expects a filepath to a 3D model.
        SharedModel(const std::string& path, bool gamma = false, bool flipUVs = false);


        unsigned int getNumberOfMeshes() const;

    private:
        
        
        // processes a node in a recursive fashion.
        // Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
        void processNode(aiNode* node, const aiScene* scene);

        Mesh processMesh(aiMesh* mesh, const aiScene* scene);

        bool checkMetalnessRoughnessSingleTexture(const aiScene* scene, aiMaterial* mat);

        // checks all material textures of a given type and loads the textures if they're not loaded yet.
        // the required info is returned as a Texture struct.
        std::vector<Texture> loadMaterialTextures(const aiScene* scene, aiMaterial* mat, aiTextureType type, const std::string& typeName);

    protected:

        unsigned int m_numberOfMeshes{};

        

        // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
        void loadModel(const std::string& path, bool flipUVs = false);

        mutable std::mutex textureMutex;
    };


    class Model : public SharedModel
    {
    public:
        glm::vec3 position{};
        glm::vec3 rotation{};
        glm::vec3 scale{};

        Model() = default;
		~Model() = default;

        // constructor, expects a filepath to a 3D model.
        Model(const std::string& _path, bool _gamma = false, bool _flipUVs = false, const glm::vec3& _position = glm::vec3());

        Model(const std::shared_ptr<SharedModel>& _shared_model, bool _gamma = false, bool _flipUVs = false, const glm::vec3& _position = glm::vec3());

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::mat4& transformMatrix, Transform& localTransform);

        void clean();

        unsigned int getNumberOfMeshes() const;


    private:
        // for shared model only (loaded one time, drawn multiple times)
        std::shared_ptr<SharedModel> m_shared_model{};
    };
}
