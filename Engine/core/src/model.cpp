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


engine::SharedModel::SharedModel(bool gamma, bool flipUVs)
{

}

engine::SharedModel::SharedModel(const std::string& path, bool gamma, bool flipUVs)
{
    assert(!path.empty() && "Model path is empty !");

    loadModel(path, flipUVs);
}

// constructor, expects a filepath to a 3D model.
engine::Model::Model(const std::string& _path, bool _gamma, bool _flipUVs, const glm::vec3& _position)
    : SharedModel(_path, _gamma, _flipUVs), position(_position)
{
}

// constructor, expects a model (for sharing)
engine::Model::Model(const std::shared_ptr<SharedModel>& _shared_model, bool _gamma, bool _flipUVs, const glm::vec3& _position)
    : SharedModel(_gamma, _flipUVs), m_shared_model(_shared_model), position(_position)
{
}

void engine::SharedModel::loadModel(const std::string& path, bool flipUVs)
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
        logger.error("Model loading error : {}", importer.GetErrorString());
        return;
    }

    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);

    m_numberOfMeshes += scene->mNumMeshes;

    // Stop the timer
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Print the time taken
    logger.info("Loading model {} : {} milliseconds", path, duration.count());
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void engine::SharedModel::processNode(aiNode* node, const aiScene* scene)
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

engine::Mesh engine::SharedModel::processMesh(aiMesh* mesh, const aiScene* scene)
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
    //std::cout << "diffuseMaps " << diffuseMaps.size() << std::endl;
    for (auto& texture : diffuseMaps) { textures.push_back(std::move(texture)); }
    // 2. specular maps
    std::vector<engine::Texture> specularMaps = loadMaterialTextures(scene, material, aiTextureType_SPECULAR, "texture_specular"); // map_Ks
    //std::cout << "specularMaps " << specularMaps.size() << std::endl;
    for (auto& texture : specularMaps) { textures.push_back(std::move(texture)); }
    // 3. normal maps
    std::vector<engine::Texture> normalMaps = loadMaterialTextures(scene, material, aiTextureType_NORMALS, "texture_normal"); //map_Kn
    //std::cout << "normalMaps " << normalMaps.size() << std::endl;
    for (auto& texture : normalMaps) { textures.push_back(std::move(texture)); }
    // 4. metallic maps (now tagged as "texture_metalness_from_combined")
    std::vector<engine::Texture> metallicMaps = loadMaterialTextures(scene, material, aiTextureType_METALNESS, "texture_metalness"); //map_Pm
    //std::cout << "metallicMaps " << metallicMaps.size() << std::endl;
    for (auto& texture : metallicMaps) { textures.push_back(std::move(texture)); }
    // 5. roughness maps (now tagged as "texture_roughness_from_combined")
    std::vector<engine::Texture> roughnessMaps = loadMaterialTextures(scene, material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness"); //map_Pr
    //std::cout << "roughnessMaps " << roughnessMaps.size() << std::endl;
    for (auto& texture : roughnessMaps) { textures.push_back(std::move(texture)); }
    // 6. ambient occlusion maps
    std::vector<engine::Texture> ambientOcclusionMaps = loadMaterialTextures(scene, material, aiTextureType_SHEEN, "texture_ao"); // map_Ps (use sheen but hack) aiTextureType_LIGHTMAP
    //std::cout << "ambientOcclusionMaps " << ambientOcclusionMaps.size() << std::endl;
    for (auto& texture : ambientOcclusionMaps) { textures.push_back(std::move(texture)); }
    // 7. height maps
    std::vector<engine::Texture> heightMaps = loadMaterialTextures(scene, material, aiTextureType_HEIGHT, "texture_height"); // bump
    //std::cout << "heightMaps " << heightMaps.size() << std::endl;
    for (auto& texture : heightMaps) { textures.push_back(std::move(texture)); }
    // 8. emissive maps
    std::vector<engine::Texture> emissiveMaps = loadMaterialTextures(scene, material, aiTextureType_EMISSIVE, "texture_emissive"); // map_Ke
    //std::cout << "emissiveMaps " << emissiveMaps.size() << std::endl;
    for (auto& texture : emissiveMaps) { textures.push_back(std::move(texture)); }

    float shininess{};
    if (AI_SUCCESS != aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess) || shininess <= 0.0f)
    {
        // if unsuccessful set a default
        shininess = 32.0f;
    }

    // Create Material
    auto meshMaterial = std::make_shared<Material>(std::move(textures), shininess);

    // return a mesh object created from the extracted mesh data
    return Mesh{ std::move(vertices), std::move(indices), meshMaterial };
}

bool engine::SharedModel::checkMetalnessRoughnessSingleTexture(const aiScene* scene, aiMaterial* mat)
{
    aiString str1{};
    aiString str2{};
    
    for (unsigned int i = 0; i < mat->GetTextureCount(aiTextureType_METALNESS); i++)
    {
        mat->GetTexture(aiTextureType_METALNESS, i, &str1);
    }

    for (unsigned int j = 0; j < mat->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS); j++)
    {
        mat->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, j, &str2);
    }

    return str1 == str2;
}


std::vector<engine::Texture> engine::SharedModel::loadMaterialTextures(const aiScene* scene, aiMaterial* mat, aiTextureType type, const std::string& typeName)
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
                // If this is a metallic/roughness texture, mark it as such
                if (type == aiTextureType_METALNESS || type == aiTextureType_DIFFUSE_ROUGHNESS)
                {
                    engine::Texture texture{ loaded.id, typeName, str.C_Str() };

                    bool singleTexture = checkMetalnessRoughnessSingleTexture(scene, mat);
                    if (singleTexture)
                    {
                        // Tag the texture as metallic or roughness
                        if (type == aiTextureType_METALNESS)
                            texture.type = "texture_metalness_from_combined";
                        else if (type == aiTextureType_DIFFUSE_ROUGHNESS)
                            texture.type = "texture_roughness_from_combined";
                    }

                    textures.push_back(std::move(texture));
                }
                else
                {
                    textures.emplace_back(loaded.id, typeName, str.C_Str());
                }

                //std::cout << "Texture " << loaded.path.c_str() << " was already loaded (reuse)" << std::endl;

                skip = true;
                break;
            }
        }

        if (!skip)
        {
            engine::Texture texture{ 0, typeName, str.C_Str() };
            if (str.C_Str()[0] == '*')
            {
                // Embedded texture from model
                int index = std::atoi(str.C_Str() + 1);
                const aiTexture* aiTex = scene->mTextures[index];
                if (aiTex->mHeight == 0)
                {
                    texture.id = engine::Texture::loadTextureFromMemory(reinterpret_cast<unsigned char*>(aiTex->pcData), aiTex->mWidth, aiTex->mFilename.C_Str());
                }
                else
                {
                    texture.id = engine::Texture::loadUncompressedTexture(reinterpret_cast<const unsigned char*>(aiTex->pcData), aiTex->mWidth, aiTex->mHeight);
                }
            }
            else
            {
                // Texture from file
                texture.id = engine::Texture::loadTextureFromFile(str.C_Str(), this->directory);
            }

            if (type == aiTextureType_METALNESS || type == aiTextureType_DIFFUSE_ROUGHNESS)
            {
                bool singleTexture = checkMetalnessRoughnessSingleTexture(scene, mat);
                if (singleTexture)
                {
                    // Tag the texture if it's metallic or roughness
                    if (type == aiTextureType_METALNESS)
                        texture.type = "texture_metalness_from_combined";
                    else if (type == aiTextureType_DIFFUSE_ROUGHNESS)
                        texture.type = "texture_roughness_from_combined";
                }
            }

            textures.emplace_back(texture.id, texture.type, texture.path);
            textures_loaded.emplace_back(texture.id, texture.type, texture.path);
        }
    }
    return textures;
}

// draws the model, and thus all its meshes
void engine::Model::draw(Shader& shader, const glm::mat4& transformMatrix, Transform& localTransform)
{
    position = localTransform.getLocalPosition();
    rotation = localTransform.getLocalRotation();
    scale = localTransform.getLocalScale();
    
    if (!m_shared_model)
    {
        for (auto& mesh : meshes)
        {
            mesh.draw(shader, transformMatrix);
        }
    }
    else
    {
		// shared model, loaded one time, drawn multiple times
        for (auto& mesh : m_shared_model->meshes)
        {
            mesh.draw(shader, transformMatrix);
        }
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

unsigned int engine::SharedModel::getNumberOfMeshes() const
{
    return m_numberOfMeshes;
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