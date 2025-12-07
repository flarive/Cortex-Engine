#include "../../include/models/shared_model.h"

#include "../../include/managers/log_manager.h"
#include "../../include/tools/file_system.h"
#include "../../include/tools/helpers.h"

#include "../../include/models/assimp_glm_helpers.h"



engine::SharedModel::SharedModel(bool gamma, bool flipUVs)
{

}

engine::SharedModel::SharedModel(const std::string& path, bool gamma, bool flipUVs)
{
    assert(!path.empty() && "Model path is empty !");

    loadModel(path, flipUVs);
}

engine::SharedModel::SharedModel(const std::string& path, const std::shared_ptr<Material>& material, bool gamma, bool flipUVs)
{
    assert(!path.empty() && "Model path is empty !");

    assert(material && "Material is not defined !");

	m_customMaterial = material;

    loadModel(path, flipUVs);
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

    // check for bones or not
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];
        if (mesh->HasBones()) {
            m_hasBones = true;
            break;
        }
    }

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


    std::shared_ptr<Material> meshMaterial{};

    // get textures
    
    // 1. diffuse maps
    std::vector<engine::Texture> diffuseMaps = loadMaterialTextures(scene, material, aiTextureType_DIFFUSE, "texture_diffuse"); // map_Kd
    for (auto& texture : diffuseMaps) { textures.push_back(std::move(texture)); }
    // 2. specular maps
    std::vector<engine::Texture> specularMaps = loadMaterialTextures(scene, material, aiTextureType_SPECULAR, "texture_specular"); // map_Ks
    for (auto& texture : specularMaps) { textures.push_back(std::move(texture)); }
    // 3. normal maps
    std::vector<engine::Texture> normalMaps = loadMaterialTextures(scene, material, aiTextureType_NORMALS, "texture_normal"); //map_Kn
    for (auto& texture : normalMaps) { textures.push_back(std::move(texture)); }
    // 4. metallic maps (now tagged as "texture_metalness_from_combined")
    std::vector<engine::Texture> metallicMaps = loadMaterialTextures(scene, material, aiTextureType_METALNESS, "texture_metalness"); //map_Pm
    for (auto& texture : metallicMaps) { textures.push_back(std::move(texture)); }
    // 5. roughness maps (now tagged as "texture_roughness_from_combined")
    std::vector<engine::Texture> roughnessMaps = loadMaterialTextures(scene, material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness"); //map_Pr
    for (auto& texture : roughnessMaps) { textures.push_back(std::move(texture)); }
    // 6. ambient occlusion maps
    std::vector<engine::Texture> ambientOcclusionMaps = loadMaterialTextures(scene, material, aiTextureType_SHEEN, "texture_ao"); // map_Ps (use sheen but hack) aiTextureType_LIGHTMAP
    for (auto& texture : ambientOcclusionMaps) { textures.push_back(std::move(texture)); }
    // 7. height maps
    std::vector<engine::Texture> heightMaps = loadMaterialTextures(scene, material, aiTextureType_HEIGHT, "texture_height"); // bump
    for (auto& texture : heightMaps) { textures.push_back(std::move(texture)); }
    // 8. emissive maps
    std::vector<engine::Texture> emissiveMaps = loadMaterialTextures(scene, material, aiTextureType_EMISSIVE, "texture_emissive"); // map_Ke
    for (auto& texture : emissiveMaps) { textures.push_back(std::move(texture)); }

    float shininess{};
    if (AI_SUCCESS != aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess) || shininess <= 0.0f)
    {
        // if unsuccessful set a default
        shininess = 32.0f;
    }

    

    if (m_customMaterial)
    {
		// use a user defined material
        meshMaterial = m_customMaterial;
	}
    else
    {
        // use material data embedded into model file
        if (textures.size() > 0)
        {
            meshMaterial = std::make_shared<Material>(std::move(textures), shininess);
            meshMaterial->setAllTexturesLoaded(true);
        }
        else
        {
            meshMaterial = std::make_shared<Material>
                (Color(ambient.r, ambient.g, ambient.b, ambient.a),
                    Color(diffuse.r, diffuse.g, diffuse.b, diffuse.a),
                    Color(specular.r, specular.g, specular.b, specular.a), shininess);
        }
    }

    if (m_hasBones)
        extractBoneWeightForVertices(vertices, mesh, scene);

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

unsigned int engine::SharedModel::getNumberOfMeshes() const
{
    return m_numberOfMeshes;
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


