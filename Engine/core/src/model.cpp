#include "../include/model.h"

#include "../include/texture.h"

#include "../include/misc/log_manager.h"
#include "../include/tools/file_system.h"
#include "../include/tools/helpers.h"

#include "SOIL2.h"

#include <omp.h> // Include OpenMP header

#include <format>
#include <chrono>
#include <future>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>  // For glm::rotation and glm::eulerAngles



// constructor, expects a filepath to a 3D model.
engine::Model::Model(const std::string& path, bool gamma, bool flipUVs) : gammaCorrection(gamma)
{
    assert(!path.empty() && "Model path is empty !");

    loadModel(path, flipUVs);
}


void engine::Model::loadModel(const std::string& path, bool flipUVs)
{
    // Start the timer
    auto start = std::chrono::high_resolution_clock::now();
    
    // read file via ASSIMP
    Assimp::Importer importer;

    auto flags = 0x0;
    flags |= aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace;

    if (flipUVs)
        flags |= aiProcess_FlipUVs;


    const aiScene* scene = importer.ReadFile(path, flags);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);

    //numberOfMeshes += scene->mRootNode->mNumMeshes;
    numberOfMeshes += scene->mNumMeshes;

    // Stop the timer
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Print the time taken
    logger.info("Loading model {} : {} milliseconds", path, duration.count());
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void engine::Model::processNode(aiNode* node, const aiScene* scene)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

engine::Mesh engine::Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    // Data to fill
    std::vector<engine::Vertex> vertices{}; // Pre-allocate space
    std::vector<unsigned int> indices{};
    std::vector<engine::Texture> textures{};
    
    // Reserve space to avoid reallocations
    indices.reserve(mesh->mNumFaces * 3); // Assuming each face is a triangle
    vertices.reserve(mesh->mNumVertices);

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        glm::vec3 vector{}; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.

        // positions
        engine::Vertex vertex(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));

        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec{};
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.bitangent = vector;
        }
        else
        {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.emplace_back(std::move(vertex));
    }



    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.emplace_back(face.mIndices[j]);
    }


    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    std::vector<engine::Texture> diffuseMaps = loadMaterialTextures(scene, material, aiTextureType_DIFFUSE, "texture_diffuse"); // map_Kd
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<engine::Texture> specularMaps = loadMaterialTextures(scene, material, aiTextureType_SPECULAR, "texture_specular"); // map_Ks
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<engine::Texture> normalMaps = loadMaterialTextures(scene, material, aiTextureType_NORMALS, "texture_normal"); //map_Kn
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. metallic maps
    std::vector<engine::Texture> metallicMaps = loadMaterialTextures(scene, material, aiTextureType_METALNESS, "texture_metalness"); //map_Pm
    textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());
    // 5. roughness maps
    std::vector<engine::Texture> roughnessMaps = loadMaterialTextures(scene, material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness"); //map_Pr
    textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
    // 6. ambient occlusion maps
    std::vector<engine::Texture> ambientOcclusionMaps = loadMaterialTextures(scene, material, aiTextureType_SHEEN, "texture_ao"); // map_Ps (use sheen but hack) aiTextureType_LIGHTMAP
    textures.insert(textures.end(), ambientOcclusionMaps.begin(), ambientOcclusionMaps.end());
    // 7. height maps
    std::vector<engine::Texture> heightMaps = loadMaterialTextures(scene, material, aiTextureType_HEIGHT, "texture_height"); // bump
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    // 8. emissive maps
    std::vector<engine::Texture> emissiveMaps = loadMaterialTextures(scene, material, aiTextureType_EMISSIVE, "texture_emissive"); // map_Ke
    textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());

    float shininess{};
    if (AI_SUCCESS != aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess) || shininess <= 0.0f)
    {
        // if unsuccessful set a default
        shininess = 32.0f;
    }

    // Create Material
    auto meshMaterial = std::make_shared<Material>(textures, shininess);

    // return a mesh object created from the extracted mesh data
    return Mesh{ std::move(vertices), std::move(indices), meshMaterial };
}

std::vector<engine::Texture> engine::Model::loadMaterialTextures(const aiScene* scene, aiMaterial* mat, aiTextureType type, const std::string& typeName)
{
    std::vector<engine::Texture> textures{};
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str{};
        mat->GetTexture(type, i, &str);

        // Check if texture was already loaded
        bool skip = false;
        for (const auto& loaded : textures_loaded)
        {
            if (std::strcmp(loaded.path.c_str(), str.C_Str()) == 0)
            {
                textures.push_back(loaded);
                skip = true;
                break;
            }
        }

        if (!skip)
        {
            engine::Texture texture{};
            texture.type = typeName;
            texture.path = str.C_Str();

            if (str.C_Str()[0] == '*')
            {
                // Embedded texture
                int index = std::atoi(str.C_Str() + 1); // "*0" -> 0
                const aiTexture* aiTex = scene->mTextures[index];

                if (aiTex->mHeight == 0)
                {
                    // Compressed texture (e.g., JPEG/PNG blob)
                    logger.info("Loading model embedded texture {} ({})", aiTex->mFilename.C_Str(), str.C_Str());
                    texture.id = engine::Texture::loadTextureFromMemory(reinterpret_cast<unsigned char*>(aiTex->pcData), aiTex->mWidth);
                }
                else
                {
                    // Uncompressed (e.g., RGBA32 format)
                    logger.info("Loading model uncompressed embedded texture {} ({})", aiTex->mFilename.C_Str(), str.C_Str());
                    texture.id = engine::Texture::loadUncompressedTexture(reinterpret_cast<const unsigned char*>(aiTex->pcData), aiTex->mWidth, aiTex->mHeight);
                }
            }
            else
            {
                // Texture from file
                logger.info("Loading model texture {}", str.C_Str());
                texture.id = engine::Texture::loadTextureFromFile(str.C_Str(), this->directory);
            }

            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }

    return textures;
}



// draws the model, and thus all its meshes
void engine::Model::draw(Shader& shader, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
    // Compute quaternion from Euler rotation
    glm::quat quaternion = glm::quat(glm::radians(rotation)); // Euler (XYZ) -> quaternion
    quaternion = glm::normalize(quaternion);

    // Compose model matrix
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model *= glm::mat4_cast(quaternion); // apply rotation
    model = glm::scale(model, scale);

    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i].draw(shader, model);
    }
}


// draws the model, and thus all its meshes
void engine::Model::draw(Shader& shader, const glm::mat4& worldTransform)
{
    for (auto& mesh : meshes)
    {
        mesh.draw(shader, worldTransform);
    }
}

void engine::Model::clean()
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i].clean();
    }

    textures_loaded.clear();
    meshes.clear();
}
