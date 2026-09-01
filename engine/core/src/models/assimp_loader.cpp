#include "../../include/models/assimp_loader.h"

#include "../../include/managers/log_manager.h"
#include "../../include/managers/filesystem_manager.h"
#include "../../include/managers/texture_manager.h"

#include "../../include/models/assimp_glm_helpers.h"

#include "../../include/singleton.h"

void engine::AssimpMeshLoader::loadModel(const std::string& path, bool flipUVs)
{
    // read file via ASSIMP
    Assimp::Importer importer;

    auto flags = 0x0;
    flags |= aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace;

    if (flipUVs)
        flags |= aiProcess_FlipUVs;

    // retrieve the directory path of the filepath
    m_directory = FileSystemManager::getDirectoryPath(path);

    // retrieve the filename of the filepath
    m_filename = FileSystemManager::getFileName(path);

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

    // m_boneInfoMap is now filled
    if (m_hasBones)
    {
        m_hasAnimations = false; // because no BoneAnimation was loaded

		// build full skeleton
        buildSkeletonFromAssimpScene(scene);
        
		// build a neutral bind-pose animation
        computeBindPoseMatrices();

        // Meshes can now safely receive bind-pose matrices
        for (unsigned int i = 0; i < m_meshes.size() ; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            std::shared_ptr<Mesh> mesh = m_meshes[i];
            mesh->bindPoseMatrices = m_finalBindPoseMatrices;
            mesh->hasBones = m_hasBones;
            mesh->hasAnimations = m_hasAnimations;
        }
    }

    m_numberOfMeshes += scene->mNumMeshes;
}


// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void engine::AssimpMeshLoader::processNode(aiNode* node, const aiScene* scene)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(processMesh(mesh, scene));
    }

    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

std::shared_ptr<engine::Mesh> engine::AssimpMeshLoader::processMesh(aiMesh* mesh, const aiScene* scene)
{
    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    SceneSettings& sceneSettings = singleton->sceneSettings();

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
    aiMaterial* aimaterial = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN



    // get colors
    aiColor4D ambient, diffuse, specular;
    aiGetMaterialColor(aimaterial, AI_MATKEY_COLOR_AMBIENT, &ambient);
    aiGetMaterialColor(aimaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
    aiGetMaterialColor(aimaterial, AI_MATKEY_COLOR_SPECULAR, &specular);

    //if (m_customMaterial) // TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //{
    //    // use a user defined material
    //    m_materials.push_back(m_customMaterial);
    //}
    //else
    //{

    std::shared_ptr<Material> material{};

    if (sceneSettings.method == RenderMethod::PBR)
    {
        material = loadPBRMaterial(scene, aimaterial);
    }
    else
    {
        material = loadBlinnPhongMaterial(scene, aimaterial);
    }
    
    m_materials.push_back(material);
    //}

    // load all textures asynchronously
    if (m_materials.back()->hasTextureMap())
        m_materials.back()->loadTexturesAsync(false);

    // load bones
    if (m_hasBones)
        extractBoneWeightForVertices(vertices, mesh, scene);

    // return a mesh object created from the extracted mesh data
    return std::make_shared<Mesh>(mesh->mName.C_Str(), std::move(vertices), std::move(indices), m_materials.back());
}

void engine::AssimpMeshLoader::extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
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


/// <summary>
/// ARM combined textures (ao + metalness + roughness)
/// </summary>
/// <param name="scene"></param>
/// <param name="mat"></param>
/// <returns></returns>
bool engine::AssimpMeshLoader::isARMSingleTexture(const aiScene* scene, aiMaterial* mat)
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
bool engine::AssimpMeshLoader::isMRSingleTexture(const aiScene* scene, aiMaterial* mat)
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

std::shared_ptr<engine::Material> engine::AssimpMeshLoader::loadPBRMaterial(const aiScene* scene, aiMaterial* mat)
{
    std::shared_ptr<Material> material{};

    std::string texDiffuseFullPath{};
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

    
    texDiffuseFullPath = getTexture(scene, mat, aiTextureType::aiTextureType_DIFFUSE);
    texNormalFullPath = getTexture(scene, mat, aiTextureType::aiTextureType_NORMALS);
    texMetalnessFullPath = getTexture(scene, mat, aiTextureType::aiTextureType_METALNESS);
    texRoughnessFullPath = getTexture(scene, mat, aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS);
    texAmbientOcclusionFullPath = getTexture(scene, mat, aiTextureType::aiTextureType_AMBIENT_OCCLUSION);

    if (texAmbientOcclusionFullPath.empty())
    {
        texAmbientOcclusionFullPath = getTexture(scene, mat, aiTextureType::aiTextureType_LIGHTMAP);
    }

    if (texAmbientOcclusionFullPath.empty())
    {
        texAmbientOcclusionFullPath = getTexture(scene, mat, aiTextureType::aiTextureType_SHEEN);
    }

    texHeightFullPath = getTexture(scene, mat, aiTextureType::aiTextureType_HEIGHT);
    texEmissiveFullPath = getTexture(scene, mat, aiTextureType::aiTextureType_EMISSIVE);

    if (useARMTexture = isARMSingleTexture(scene, mat))
    {
        texArmFullPath = texAmbientOcclusionFullPath;
    }
    else if (useMRTexture = isMRSingleTexture(scene, mat))
    {
        texRmFullPath = texMetalnessFullPath;
    }
   


    aiColor4D aiBaseColorFactor(1, 1, 1, 1);
    mat->Get(AI_MATKEY_BASE_COLOR, aiBaseColorFactor);
    Color baseColorFactor{ aiBaseColorFactor.r, aiBaseColorFactor.g, aiBaseColorFactor.b, aiBaseColorFactor.a };

    float shininess = 1.0f; // TODO, remove shininess from PBR material !

    if (useARMTexture && !texArmFullPath.empty())
    {
        material = std::make_shared<PBRMaterial>(CombinedTexture::ARM, baseColorFactor,
            texDiffuseFullPath, texNormalFullPath, texArmFullPath, texHeightFullPath, texEmissiveFullPath, shininess);
    }
    else if (useMRTexture && !texRmFullPath.empty())
    {
        material = std::make_shared<PBRMaterial>(CombinedTexture::RM, baseColorFactor,
            texDiffuseFullPath, texNormalFullPath, texRmFullPath, texHeightFullPath, texEmissiveFullPath, shininess);
    }
    else
    {
        material = std::make_shared<PBRMaterial>(baseColorFactor,
            texDiffuseFullPath, texNormalFullPath, texMetalnessFullPath, texRoughnessFullPath, texAmbientOcclusionFullPath, texHeightFullPath, texEmissiveFullPath, shininess);
    }

    if (!material->hasTextureMap())
    {
        material = std::make_shared<PBRMaterial>(baseColorFactor);
    }

    material->setName(mat->GetName().C_Str());

    return material;
}

std::shared_ptr<engine::Material> engine::AssimpMeshLoader::loadBlinnPhongMaterial(const aiScene* scene, aiMaterial* mat)
{
    float shininess = 1.0f;

    std::string texDiffuseFullPath{};
    std::string texSpecularFullPath{};
    std::string texNormalFullPath{};
    std::string texHeightFullPath{};
    std::string texEmissiveFullPath{};


    texDiffuseFullPath = getTexture(scene, mat, aiTextureType::aiTextureType_DIFFUSE);
    texSpecularFullPath = getTexture(scene, mat, aiTextureType::aiTextureType_SPECULAR);
    texNormalFullPath = getTexture(scene, mat, aiTextureType::aiTextureType_NORMALS);
    


    aiColor4D aiBaseColorFactor(1, 1, 1, 1);
    mat->Get(AI_MATKEY_BASE_COLOR, aiBaseColorFactor);
    Color baseColorFactor{ aiBaseColorFactor.r, aiBaseColorFactor.g, aiBaseColorFactor.b, aiBaseColorFactor.a };

    // TODO, get shininess from assimp

    std::shared_ptr<Material> material = std::make_shared<BlinnPhongMaterial>(baseColorFactor, texDiffuseFullPath, texSpecularFullPath, texNormalFullPath, texHeightFullPath, texEmissiveFullPath, shininess);

    material->setName(mat->GetName().C_Str());

    return material;
}

std::string engine::AssimpMeshLoader::getTexture(const aiScene* scene, aiMaterial* mat, aiTextureType type)
{
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str{};
        mat->GetTexture(type, i, &str);

        std::string filename = str.C_Str(); // safe conversion
        std::string path = std::format("{}\\{}", this->m_directory, filename);

        // Check if texture was already loaded
        bool skip = std::find(m_requestLoadingTextures.begin(), m_requestLoadingTextures.end(), path) != m_requestLoadingTextures.end();

        if (skip)
            return path;

        if (filename[0] == '*')
        {
            // Embedded texture from model, load them synchronously
            int index = std::atoi(str.C_Str() + 1);
            const aiTexture* aiTex = scene->mTextures[index];


            TextureUploadResult result;

            if (aiTex->mHeight == 0)
            {
                // embedded and compressed
                result = engine::TextureManager::loadTextureFromMemory(reinterpret_cast<unsigned char*>(aiTex->pcData), aiTex->mWidth, aiTex->mFilename.C_Str());
            }
            else
            {
                // embedded and uncompressed
                result = engine::TextureManager::loadUncompressedTexture(reinterpret_cast<const unsigned char*>(aiTex->pcData), aiTex->mWidth, aiTex->mHeight);
            }

            // Cache result
            engine::TextureManagerInternal::textureIDCache[path] = result.textureID;
            engine::TextureManagerInternal::textureDataCache[path] = TextureData{ result.textureID, reinterpret_cast<unsigned char*>(aiTex->pcData), path, result.width, result.height, result.nbComponents, result.thumbnailLevel };
        }

        // just to avoid loading 2 times the same texture path
        m_requestLoadingTextures.push_back(path);

        return path;
    }

    return "";
}

void engine::AssimpMeshLoader::buildSkeletonFromAssimpScene(const aiScene* scene)
{
    m_skeleton.clear();
    m_skeleton.reserve(m_boneInfoMap.size());

    std::unordered_map<std::string, int> boneIndexMap;

    std::function<void(const aiNode*, int)> traverse =
        [&](const aiNode* node, int parentIndex)
        {
            std::string nodeName = node->mName.C_Str();
            auto it = m_boneInfoMap.find(nodeName);

            if (it != m_boneInfoMap.end())
            {
                SkeletonBone bone{};
                bone.name = nodeName;
                bone.parentIndex = parentIndex;
                bone.localBindTransform = AssimpGLMHelpers::ConvertMatrixToGLMFormat(node->mTransformation);
                bone.offset = it->second.offset;

                int newIndex = (int)m_skeleton.size();
                m_skeleton.push_back(bone);
                boneIndexMap[nodeName] = newIndex;

                parentIndex = newIndex;
            }

            for (unsigned int i = 0; i < node->mNumChildren; i++)
                traverse(node->mChildren[i], parentIndex);
        };

    traverse(scene->mRootNode, -1);
}

glm::mat4 engine::AssimpMeshLoader::computeGlobalFromSkeleton(int index)
{
    glm::mat4 global = m_skeleton[index].localBindTransform;

    int parent = m_skeleton[index].parentIndex;
    while (parent != -1)
    {
        global = m_skeleton[parent].localBindTransform * global;
        parent = m_skeleton[parent].parentIndex;
    }

    return global;
}

void engine::AssimpMeshLoader::computeBindPoseMatrices()
{
    m_finalBindPoseMatrices.clear();
    m_finalBindPoseMatrices.reserve(m_skeleton.size());

    for (int i = 0; i < m_skeleton.size(); i++)
    {
        glm::mat4 global = computeGlobalFromSkeleton(i);
        glm::mat4 offset = m_skeleton[i].offset;

        m_finalBindPoseMatrices.push_back(global * offset);
    }
}


engine::AssimpMeshLoader::~AssimpMeshLoader()
{
    logger.trace("AssimpMeshLoader destructor called");
}