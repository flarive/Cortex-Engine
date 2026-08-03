#include "../../include/models/shared_model.h"

#include "../../include/managers/texture_manager.h"
#include "../../include/managers/log_manager.h"
#include "../../include/managers/filesystem_manager.h"
#include "../../include/tools/helpers.h"

#include "../../include/models/assimp_glm_helpers.h"

#include "../../include/singleton.h"

#include <filesystem>
#include <string>

engine::SharedModel::SharedModel(bool _gamma, bool _flipUV)
    : gammaCorrection(_gamma), flipUV(_flipUV)
{
    logger.trace("SharedModel constructor called");
}

engine::SharedModel::SharedModel(const std::string& _path, bool _gamma, bool _flipUV)
    : gammaCorrection(_gamma), flipUV(_flipUV)
{
    logger.trace("SharedModel constructor called");
    
    assert(!_path.empty() && "Model path is empty !");

    loadModel(_path, _flipUV);
}

engine::SharedModel::SharedModel(const std::string& _path, const std::shared_ptr<Material>& _material, bool _gamma, bool _flipUV)
    : gammaCorrection(_gamma), flipUV(_flipUV), m_customMaterial(_material)
{
    logger.trace("SharedModel constructor called");

    assert(!_path.empty() && "Model path is empty !");

    assert(_material && "Material is not defined !");

    loadModel(_path, _flipUV);
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




    // retrieve the directory path of the filepath
    m_directory = FileSystemManager::getDirectoryPath(path);

    // retrieve the filename of the filepath
    m_filename = FileSystemManager::getFilename(path);

    


    const aiScene* scene = importer.ReadFile(path, flags);

    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        logger.error("Model loading error : {}", importer.GetErrorString());
        return;
    }





    // check for bones or not
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];
        if (mesh->HasBones())
            m_hasBones = true;

        m_numberOfVertices += mesh->mNumVertices;
    }

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);

    m_numberOfMeshes += scene->mNumMeshes;


    // Stop the timer
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Print the time taken
    logger.info("Loading model {} : {} milliseconds", FileSystemManager::getShortenedPath(path), duration.count());
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
        Vertex vertex{ glm::vec3(0.0f, 0.0f, 0.0f) };

        glm::vec3 vector{}; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.


        if (m_hasBones)
            setVertexBoneDataToDefault(vertex);

        // positions
        vertex.position = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);

        // normals
        if (mesh->HasNormals())
            vertex.normal = AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i]);

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



    // get colors
    aiColor4D ambient, diffuse, specular;
    aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambient);
    aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
    aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular);



	// load all textures asynchronously
    m_material = loadMaterialTextures(scene, material);
    if (m_material->hasDiffuseMap())
        m_material->loadTexturesAsync();



    if (m_hasBones)
        extractBoneWeightForVertices(vertices, mesh, scene);

    // return a mesh object created from the extracted mesh data
    return Mesh{ std::move(vertices), std::move(indices), m_material };
}

/// <summary>
/// ARM combined textures (ao + metalness + roughness)
/// </summary>
/// <param name="scene"></param>
/// <param name="mat"></param>
/// <returns></returns>
bool engine::SharedModel::isARMSingleTexture(const aiScene* scene, aiMaterial* mat)
{
    aiString str1{};
    aiString str2{};
    aiString str3{};

    for (unsigned int i = 0; i < mat->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION); i++)
    {
        mat->GetTexture(aiTextureType_AMBIENT_OCCLUSION, i, &str1);
    }

    if (str1.length == 0)
    {
        for (unsigned int i = 0; i < mat->GetTextureCount(aiTextureType_LIGHTMAP); i++)
        {
            mat->GetTexture(aiTextureType_LIGHTMAP, i, &str1);
        }
    }

    if (str1.length == 0)
    {
        for (unsigned int i = 0; i < mat->GetTextureCount(aiTextureType_SHEEN); i++)
        {
            mat->GetTexture(aiTextureType_SHEEN, i, &str1);
        }
    }

    for (unsigned int i = 0; i < mat->GetTextureCount(aiTextureType_METALNESS); i++)
    {
        mat->GetTexture(aiTextureType_METALNESS, i, &str2);
    }

    for (unsigned int j = 0; j < mat->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS); j++)
    {
        mat->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, j, &str3);
    }

    return str1 == str2 && str2 == str3;
}

/// <summary>
/// MR combined textures (metalness + roughness)
/// </summary>
/// <param name="scene"></param>
/// <param name="mat"></param>
/// <returns></returns>
bool engine::SharedModel::isMRSingleTexture(const aiScene* scene, aiMaterial* mat)
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

std::shared_ptr<engine::Material> engine::SharedModel::loadMaterialTextures(const aiScene* scene, aiMaterial* mat)
{
    std::shared_ptr<Material> material{};

    float shininess = 1.0f;


    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    SceneSettings& sceneSettings = singleton->sceneSettings();


    std::string texDiffuseFullPath{};
    std::string texSpecularFullPath{};
    std::string texNormalFullPath{};
    std::string texMetalnessFullPath{};
    std::string texRoughnessFullPath{};
    std::string texAmbientOcclusionFullPath{};
    std::string texHeightFullPath{};
    std::string texEmissiveFullPath{};

    std::string texArmFullPath{};
    std::string texRmFullPath{};

    bool useARMTexture = false;
	bool useMRTexture = false;

    if (sceneSettings.method == RenderMethod::PBR)
    {
        texDiffuseFullPath = getTexture(mat, aiTextureType::aiTextureType_DIFFUSE);
		texNormalFullPath = getTexture(mat, aiTextureType::aiTextureType_NORMALS);
        texMetalnessFullPath = getTexture(mat, aiTextureType::aiTextureType_METALNESS);
		texRoughnessFullPath = getTexture(mat, aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS);
		texAmbientOcclusionFullPath = getTexture(mat, aiTextureType::aiTextureType_AMBIENT_OCCLUSION);

		if (texAmbientOcclusionFullPath.empty())
		{
			texAmbientOcclusionFullPath = getTexture(mat, aiTextureType::aiTextureType_LIGHTMAP);
		}

        if (texAmbientOcclusionFullPath.empty())
        {
            texAmbientOcclusionFullPath = getTexture(mat, aiTextureType::aiTextureType_SHEEN);
        }

		texHeightFullPath = getTexture(mat, aiTextureType::aiTextureType_HEIGHT);
		texEmissiveFullPath = getTexture(mat, aiTextureType::aiTextureType_EMISSIVE);



        if (useARMTexture = isARMSingleTexture(scene, mat))
        {
            texArmFullPath = texAmbientOcclusionFullPath;
        }
        else if (useMRTexture = isMRSingleTexture(scene, mat))
        {
			texRmFullPath = texMetalnessFullPath;
        }
    }
    else
    {
        texDiffuseFullPath = getTexture(mat, aiTextureType::aiTextureType_DIFFUSE);
        texSpecularFullPath = getTexture(mat, aiTextureType::aiTextureType_SPECULAR);
		texNormalFullPath = getTexture(mat, aiTextureType::aiTextureType_NORMALS);
    }



    if (sceneSettings.method == RenderMethod::PBR)
    {
        if (useARMTexture)
        {
            material = std::make_shared<PBRMaterial>(CombinedTexture::ARM, Color(0.1f), texDiffuseFullPath, texNormalFullPath, texArmFullPath, texHeightFullPath, shininess);
		}
		else if (useMRTexture)
		{
			material = std::make_shared<PBRMaterial>(CombinedTexture::RM, Color(0.1f), texDiffuseFullPath, texNormalFullPath, texRmFullPath, texHeightFullPath, shininess);
		}
        else
        {
            material = std::make_shared<PBRMaterial>(Color(0.1f), texDiffuseFullPath, texNormalFullPath, texMetalnessFullPath, texRoughnessFullPath, texAmbientOcclusionFullPath, texHeightFullPath, shininess);
        }
	}
	else
	{
        material = std::make_shared<BlinnPhongMaterial>(Color(0.1f), texDiffuseFullPath, texSpecularFullPath, texNormalFullPath, texHeightFullPath, shininess);
	}

    return material;
}

std::string engine::SharedModel::getTexture(aiMaterial* mat, aiTextureType type) const
{
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str{};
        mat->GetTexture(type, i, &str);

        return std::format("{}/{}", this->m_directory, str.C_Str());
    }
    
    return "";
}

unsigned int engine::SharedModel::getMeshCount() const
{
    return m_numberOfMeshes;
}

unsigned int engine::SharedModel::getVertexCount() const
{
    return m_numberOfVertices;
}

void engine::SharedModel::setVertexBoneDataToDefault(Vertex& vertex)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        vertex.boneIDs[i] = -1;
        vertex.weights[i] = 0.0f;
    }
}

void engine::SharedModel::setVertexBoneData(Vertex& vertex, int boneID, float weight)
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

void engine::SharedModel::extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
    auto& boneInfoMap = m_boneInfoMap;
    int& boneCount = m_boneCounter;

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

void engine::SharedModel::reSetup()
{
    std::filesystem::path fullpath = std::filesystem::path(m_directory) / m_filename;
    std::string fullpath_str = fullpath.string();

    loadModel(fullpath_str, flipUV);
}

engine::SharedModel::~SharedModel()
{
    logger.trace("SharedModel destructor called");
}

