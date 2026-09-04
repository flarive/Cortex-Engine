#include "../../../include/animations/loaders/animation_loader.h"

#include "../../../include/managers/log_manager.h"
#include "../../../include/managers/filesystem_manager.h"


#include "../../../include/animations/loaders/assimp_animation_loader.h"
#include "../../../include/animations/loaders/tinygltf_animation_loader.h"

engine::AnimationLoader::AnimationLoader()
{
	logger.trace("AnimationLoader constructor called");
}

std::unique_ptr<engine::AnimationLoader> engine::AnimationLoader::create(const std::string& path)
{
    std::string ext = FileSystemManager::getFileExt(path);

    // Use tinyGLTF for GLTF (more features than Assimp)
    if (ext == "gltf" || ext == "glb")
        return std::make_unique<engine::TinygltfAnimationLoader>();

    // Assimp supports many formats: obj, fbx, dae, ply, 3ds, etc.
    return std::make_unique<engine::AssimpAnimationLoader>();
}

engine::Bone* engine::AnimationLoader::findBone(const std::string& name)
{
	auto iter = std::find_if(m_bones.begin(), m_bones.end(),
		[&](const Bone& bone)
		{
			return bone.getBoneName() == name;
		}
	);
	if (iter == m_bones.end()) return nullptr;
	else return &(*iter);
}


engine::Bone engine::AnimationLoader::createBone(const std::string& name, int id, std::vector<KeyPosition> positions, std::vector<KeyRotation> rotations, std::vector<KeyScale> scales)
{
	return Bone(name, id,
		std::move(positions),
		std::move(rotations),
		std::move(scales));
}


engine::AnimationLoader::~AnimationLoader()
{
	logger.trace("AnimationLoader destructor called");
}