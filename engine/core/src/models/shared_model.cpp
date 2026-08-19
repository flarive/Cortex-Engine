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
    : m_gammaCorrection(_gamma), m_flipUV(_flipUV)
{
    logger.trace("SharedModel constructor called");
}

engine::SharedModel::SharedModel(const std::string& _path, bool _gamma, bool _flipUV)
    : m_gammaCorrection(_gamma), m_flipUV(_flipUV)
{
    logger.trace("SharedModel constructor called");
    
    assert(!_path.empty() && "Model path is empty !");

    loadModel(_path, _flipUV);
}

engine::SharedModel::SharedModel(const std::string& _path, const std::shared_ptr<Material>& _material, bool _gamma, bool _flipUV)
    : m_gammaCorrection(_gamma), m_flipUV(_flipUV), m_customMaterial(_material)
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

    // ?????????????????????????????????????????????
    m_meshLoader = MeshLoader::create(path);
    m_meshLoader->loadModel(path, flipUVs);



    // Stop the timer
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Print the time taken
    logger.info("Loading model {} : {} milliseconds", FileSystemManager::getShortenedPath(path), duration.count());
}

unsigned int engine::SharedModel::getMeshCount() const
{
    if (m_meshLoader)
		return m_meshLoader->getMeshCount();
    
    return 0;
}

unsigned int engine::SharedModel::getVertexCount() const
{
    if (m_meshLoader)
        return m_meshLoader->getVertexCount();

    return 0;
}

std::vector<std::shared_ptr<engine::Mesh>>& engine::SharedModel::getMeshes()
{
	if (m_meshLoader)
		return m_meshLoader->getMeshes();

	static std::vector<std::shared_ptr<engine::Mesh>> emptyMeshes;
	return emptyMeshes;
}

void engine::SharedModel::reSetup()
{
    std::filesystem::path fullpath = std::filesystem::path(m_directory) / m_filename;
    std::string fullpath_str = fullpath.string();

    loadModel(fullpath_str, m_flipUV);
}

engine::SharedModel::~SharedModel()
{
    logger.trace("SharedModel destructor called");

    m_meshLoader.reset();
}