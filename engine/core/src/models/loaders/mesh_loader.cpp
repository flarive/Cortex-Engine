#include "../../../include/models/loaders/mesh_loader.h"
#include "../../../include/models/loaders/assimp_mesh_loader.h"
#include "../../../include/models/loaders/tinygltf_mesh_loader.h"

#include "../../../include/managers/log_manager.h"
#include "../../../include/managers/filesystem_manager.h"

#include <algorithm>


engine::MeshLoader::MeshLoader()
{
    logger.trace("MeshLoader constructor called");
}

std::unique_ptr<engine::MeshLoader> engine::MeshLoader::create(const std::string& path)
{
    std::string ext = FileSystemManager::getFileExt(path);

    // Use tinyGLTF for GLTF (more features than Assimp)
    if (ext == "gltf" || ext == "glb")
        return std::make_unique<engine::GLtfMeshLoader>();

    // Assimp supports many formats: obj, fbx, dae, ply, 3ds, etc.
    return std::make_unique<engine::AssimpMeshLoader>();
}

void engine::MeshLoader::setVertexBoneDataToDefault(Vertex& vertex)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        vertex.boneIDs[i] = -1;
        vertex.weights[i] = 0.0f;
    }
}

void engine::MeshLoader::setVertexBoneData(Vertex& vertex, int boneID, float weight)
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

int engine::MeshLoader::getSkeletonRootIndex() const
{
    if (m_skeleton)
        return m_skeleton->getRootIndex();

    return -1;
}

std::map<std::string, engine::BoneInfo>& engine::MeshLoader::getBoneInfoMap()
{
    if (m_skeleton)
        return m_skeleton->getBoneInfoMap();
    
    static std::map<std::string, engine::BoneInfo> emptyMap;
    return emptyMap ;
}

unsigned int engine::MeshLoader::getBoneCount()
{
	if (m_skeleton)
        return m_skeleton->getBoneCount();

	return 0;
}

engine::MeshLoader::~MeshLoader()
{
    logger.trace("MeshLoader destructor called");

    m_requestLoadingTextures.clear();

    // should destroy meshes manually because m_meshes is a vector of mesh pointer
    for (unsigned int i = 0; i < m_meshes.size(); i++) {
        m_meshes[i]->clean();
    }
    m_meshes.clear();
    
    // should destroy materials manually because m_materials is a vector of material pointer
    //for (unsigned int i = 0; i < m_materials.size(); i++) {
    //    m_materials[i]->clean();
    //}
    m_materials.clear(); // TODO !!!!!!!!!!!!!!!!!!!!!!!

    // ???????????????????????????????????
    //if (m_skeleton)
    //    m_skeleton->m_boneInfoMap.clear();
}
