#include "../../include/models/mesh_loader.h"
#include "../../include/models/assimp_loader.h"
#include "../../include/models/gltf_loader.h"

#include "../../include/managers/log_manager.h"

#include <algorithm>

static std::string toLower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

std::unique_ptr<engine::MeshLoader> engine::MeshLoader::create(const std::string& path)
{
    std::string ext;

    const size_t dot = path.find_last_of('.');
    if (dot != std::string::npos)
        ext = toLower(path.substr(dot + 1));

    if (ext == "gltf" || ext == "glb")
        return std::make_unique<engine::GLtfMeshLoader>();

    // Assimp supports many formats: obj, fbx, dae, ply, 3ds, etc.
    return std::make_unique<engine::AssimpMeshLoader>();
}

engine::MeshLoader::~MeshLoader()
{
    logger.trace("MeshLoader destructor called");

    for (unsigned int i = 0; i < m_meshes.size(); i++) {
        m_meshes[i]->clean();
    }

    m_requestLoadingTextures.clear();
    m_meshes.clear();
}