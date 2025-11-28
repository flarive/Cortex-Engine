#include "../../include/models/animated_model.h"
#include "../../include/models/shared_model.h"

#include "../../include/texture.h"

#include "../../include/managers/log_manager.h"
#include "../../include/tools/file_system.h"
#include "../../include/tools/helpers.h"

#include "SOIL2.h"

#include <omp.h> // Include OpenMP header

#include <format>
#include <chrono>
#include <future>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>  // For glm::rotation and glm::eulerAngles


// constructor, expects a filepath to a 3D model.
engine::AnimatedModel::AnimatedModel(const std::string& _path, bool _gamma, bool _flipUVs, const glm::vec3& _position)
    : Model(_path, _gamma, _flipUVs), position(_position)
{
}

// constructor, expects a model (for sharing)
//engine::AnimatedModel::AnimatedModel(const std::shared_ptr<SharedModel>& _shared_model, bool _gamma, bool _flipUVs, const glm::vec3& _position)
//    : Model(_gamma, _flipUVs), m_shared_model(_shared_model), position(_position)
//{
//}

// draws the model, and thus all its meshes
void engine::AnimatedModel::draw(Shader& shader, const glm::mat4& transformMatrix, Transform& localTransform)
{
    position = localTransform.getLocalPosition();
    rotation = localTransform.getLocalRotation();
    scale = localTransform.getLocalScale();

    if (!m_shared_model)
    {
        for (auto& mesh : meshes) {
            mesh.draw(shader, transformMatrix);
        }
    }
    else
    {
        // shared model, loaded one time, drawn multiple times
        for (auto& mesh : m_shared_model->meshes) {
            mesh.draw(shader, transformMatrix);
        }
    }
}

void engine::AnimatedModel::clean()
{
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].clean();
    }

    textures_loaded.clear();
    meshes.clear();
}

unsigned int engine::AnimatedModel::getNumberOfMeshes() const
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

void engine::AnimatedModel::setVertexBoneDataToDefault(Vertex& vertex)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        vertex.boneIDs[i] = -1;
        vertex.weights[i] = 0.0f;
    }
}

void engine::AnimatedModel::setVertexBoneData(Vertex& vertex, int boneID, float weight)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
    {
        if (vertex.boneIDs[i] < 0)
        {
            vertex.weights[i] = weight;
            vertex.boneIDs[i] = boneID;
            break;
        }
    }
}

void engine::AnimatedModel::extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
    auto& boneInfoMap = m_BoneInfoMap;
    int& boneCount = m_BoneCounter;

    for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            BoneInfo newBoneInfo;
            newBoneInfo.id = boneCount;
            newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
            boneInfoMap[boneName] = newBoneInfo;
            boneID = boneCount;
            boneCount++;
        }
        else
        {
            boneID = boneInfoMap[boneName].id;
        }
        assert(boneID != -1);
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId <= vertices.size());
            setVertexBoneData(vertices[vertexId], boneID, weight);
        }
    }
}

engine::Mesh engine::AnimatedModel::processMesh(aiMesh* mesh, const aiScene* scene)
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
        //engine::Vertex vertex(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
        engine::Vertex vertex{ AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]) };

        // normals
        if (mesh->HasNormals())
        {
            //vector.x = mesh->mNormals[i].x;
            //vector.y = mesh->mNormals[i].y;
            //vector.z = mesh->mNormals[i].z;
            //vertex.normal = vector;
            vertex.normal = AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i]);
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
    meshMaterial->setAllTexturesLoaded(true);

    // return a mesh object created from the extracted mesh data
    return Mesh{ std::move(vertices), std::move(indices), meshMaterial };
}