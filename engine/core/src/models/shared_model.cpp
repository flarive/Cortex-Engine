#include "../../include/models/shared_model.h"

#include "../../include/managers/texture_manager.h"
#include "../../include/managers/log_manager.h"
#include "../../include/managers/filesystem_manager.h"
#include "../../include/tools/helpers.h"

#include "../../include/models/loaders/assimp_glm_helpers.h"

#include "../../include/singleton.h"

#include <filesystem>
#include <string>

engine::SharedModel::SharedModel(bool _loadAnimation, bool _gamma, bool _flipUV)
    : m_gammaCorrection(_gamma), m_flipUV(_flipUV)
{
    logger.trace("SharedModel constructor called");
}

engine::SharedModel::SharedModel(const std::string& _path, bool _loadAnimation, bool _gamma, bool _flipUV)
    : m_filePath(_path), m_fileName(FileSystemManager::getFileName(_path)), m_gammaCorrection(_gamma), m_flipUV(_flipUV)
{
    logger.trace("SharedModel constructor called");
    
    assert(!_path.empty() && "Model path is empty !");

    loadModel(_path, _loadAnimation, _flipUV);
}

engine::SharedModel::SharedModel(const std::string& _path, const std::shared_ptr<Material>& _material, bool _loadAnimation, bool _gamma, bool _flipUV)
    : m_filePath(_path), m_fileName(FileSystemManager::getFileName(_path)), m_gammaCorrection(_gamma), m_flipUV(_flipUV), m_customMaterial(_material)
{
    logger.trace("SharedModel constructor called");

    assert(!_path.empty() && "Model path is empty !");

    assert(_material && "Material is not defined !");

    loadModel(_path, _loadAnimation, _flipUV, _material);
}

void engine::SharedModel::loadModel(const std::string& path, bool loadAnimation, bool flipUVs, std::shared_ptr<Material> customMaterial)
{
    // Start the timer
    auto start = std::chrono::high_resolution_clock::now();

    // Create the right mesh loader according file extension (tinyGLTF for GLTF otherwise Assimp)
    m_meshLoader = MeshLoader::create(path);
    m_meshLoader->loadModel(path, loadAnimation, flipUVs, customMaterial);

    // Stop the timer
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Print the time taken
    logger.info("Loading model {} : {} milliseconds", FileSystemManager::getShortenedPath(path), duration.count());
}

std::vector<std::shared_ptr<engine::Material>>& engine::SharedModel::getMaterials()
{
	if (m_meshLoader)
		return m_meshLoader->getMaterials();

	static std::vector<std::shared_ptr<engine::Material>> emptyMaterials;
	return emptyMaterials;
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

unsigned int engine::SharedModel::getBoneCount()
{
    if (m_meshLoader)
        return m_meshLoader->getBoneCount();

    return 0;
}

std::shared_ptr<engine::Skeleton> engine::SharedModel::getSkeleton()
{
    if (m_meshLoader)
        return m_meshLoader->getSkeleton();

    return nullptr;
}

const std::shared_ptr<engine::Skeleton> engine::SharedModel::getSkeleton() const
{
    if (m_meshLoader)
        return m_meshLoader->getSkeleton();

    return nullptr;
}

const std::vector<engine::SkeletonBone>& engine::SharedModel::getSkeletonBones() const
{
    if (m_meshLoader)
        if (m_meshLoader->getSkeleton())
            return m_meshLoader->getSkeleton()->getSkeletonBones();

    static std::vector<engine::SkeletonBone> emptyBones;
    return emptyBones;
}

//const std::vector<glm::mat4>& engine::SharedModel::getBindPoseMatrices() const
//{
//    if (m_meshLoader)
//    {
//        const auto& meshes = m_meshLoader->getMeshes();
//        if (!meshes.empty())
//        {
//            return meshes[0]->getBindPoseMatrices();
//        }
//    }
//
//    static std::vector<glm::mat4> emptyBindPoseMatrices;
//    return emptyBindPoseMatrices;
//}


bool engine::SharedModel::hasBones()
{
    if (m_meshLoader)
        return m_meshLoader->hasBones();

    return false;
}

bool engine::SharedModel::hasAnimations()
{
    if (m_meshLoader)
        return m_meshLoader->hasAnimations();

    return false;
}

unsigned int engine::SharedModel::getSkeletonRootIndex() const
{
    if (m_meshLoader)
        return m_meshLoader->getSkeletonRootIndex();

    return -1;
}

void engine::SharedModel::reSetup()
{
    loadModel(m_filePath, m_flipUV);
}

engine::SharedModel::~SharedModel()
{
    logger.trace("SharedModel destructor called");

    m_meshLoader.reset();
}