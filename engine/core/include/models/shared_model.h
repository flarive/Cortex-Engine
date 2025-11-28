#pragma once

#include "../misc/noncopyable.h"
#include "../texture.h"
#include "mesh.h"
#include "../shader.h"
#include "../transform.h"


#include <assimp/importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>
#include <mutex>

#include "SOIL2.h"

#include <omp.h> // Include OpenMP header

#include <format>
#include <chrono>
#include <future>


namespace engine
{
    class SharedModel : private NonCopyable
    {
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

        virtual Mesh processMesh(aiMesh* mesh, const aiScene* scene);

        bool checkMetalnessRoughnessSingleTexture(const aiScene* scene, aiMaterial* mat);



    protected:

        unsigned int m_numberOfMeshes{};



        // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
        void loadModel(const std::string& path, bool flipUVs = false);

        // checks all material textures of a given type and loads the textures if they're not loaded yet.
        // the required info is returned as a Texture struct.
        std::vector<Texture> loadMaterialTextures(const aiScene* scene, aiMaterial* mat, aiTextureType type, const std::string& typeName);

        mutable std::mutex textureMutex;
    };
}