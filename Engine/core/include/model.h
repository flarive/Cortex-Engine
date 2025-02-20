#pragma once

#include "misc/noncopyable.h"
#include "texture.h"
#include "mesh.h"
#include "shader.h"



#include <assimp/importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



#include <string>
#include <vector>

namespace engine
{
	class Model// : private NonCopyable
    {
    public:
        // model data 
        std::vector<Texture> textures_loaded{};	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        std::vector<Mesh> meshes{};
        std::string directory{};
        bool gammaCorrection{};

        Model() = default;
		~Model() = default;

        // constructor, expects a filepath to a 3D model.
        Model(std::string const& path, bool gamma = false);

        // draws the model, and thus all its meshes
        void draw(Shader& shader, glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), float angle = 0.0f, glm::vec3 scale = glm::vec3(1.0f));


    private:
        // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
        void loadModel(std::string const& path);

        // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
        void processNode(aiNode* node, const aiScene* scene);


        Mesh processMesh(aiMesh* mesh, const aiScene* scene);


        // checks all material textures of a given type and loads the textures if they're not loaded yet.
        // the required info is returned as a Texture struct.
        std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);
    };

    static unsigned int TextureFromFile(const char* path, const std::string& directory);
}
