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

//void engine::MeshLoader::createTrace(const std::string& name)
//{
//    m_stream.open(name, std::ios::app);
//
//    if (!m_stream.is_open()) {
//        logger.error("Failed to open trace");
//    }
//}

//void engine::MeshLoader::trace(const std::string& meshName, int vertexIndex, const Vertex& v)
//{
//    if (m_stream) {
//        m_stream << "### " << meshName << " " << vertexIndex << " " << m_vertexCounter
//            << " vpos:" << v.position.x << "/" << v.position.y << "/" << v.position.z
//            << " vnor:" << v.normal.x << "/" << v.normal.y << "/" << v.normal.z
//            << " vtan:" << v.tangent.x << "/" << v.tangent.y << "/" << v.tangent.z
//            << " vbtan:" << v.bitangent.x << "/" << v.bitangent.y << "/" << v.bitangent.z
//            << " vtex:" << v.texCoords.x << "/" << v.texCoords.y
//            << " vboneId:" << v.boneIDs[0] << "/" << v.boneIDs[1] << "/" << v.boneIDs[2] << "/" << v.boneIDs[3]
//            << " vweight:" << v.weights[0] << "/" << v.weights[1] << "/" << v.weights[2] << "/" << v.weights[3]
//            << std::endl;
//    }
//
//    m_vertexCounter++;
//}

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

    m_boneInfoMap.clear();


    //if (m_stream.is_open())
    //    m_stream.close();
}
