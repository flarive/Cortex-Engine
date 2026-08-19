#include "../../include/models/gltf_loader.h"

#include "../../include/managers/log_manager.h"
#include "../../include/managers/filesystem_manager.h"

void engine::GLtfMeshLoader::loadModel(const std::string& path, bool flipUVs)
{

}

engine::GLtfMeshLoader::~GLtfMeshLoader()
{
    logger.trace("GLtfMeshLoader destructor called");
}
