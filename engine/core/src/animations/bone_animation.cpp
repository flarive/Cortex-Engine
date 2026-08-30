#include "../../include/animations/bone_animation.h"

#include "../../include/managers/log_manager.h"


#include <assimp/importer.hpp>
#include <assimp/postprocess.h>

#include <glm/gtc/type_ptr.hpp>


engine::BoneAnimation::BoneAnimation(const std::string& animationName, const std::string& animationPath, std::shared_ptr<Model> model, float speedFactor)
	: Animation(animationName, model, speedFactor), m_filepath(animationPath)
{
	logger.trace("BoneAnimation constructor called");

	//importBoneAnimationFromAssimp(animationPath, model);
	
	importBoneAnimationFromGLTF(animationPath, model);
}

void engine::BoneAnimation::importBoneAnimationFromAssimp(const std::string& animationPath, std::shared_ptr<Model> model)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
	assert(scene && scene->mRootNode);
	aiAnimation* animation = scene->mAnimations[0];

	m_ticksPerSecond = static_cast<unsigned int>(animation->mTicksPerSecond > 0 ? animation->mTicksPerSecond : 1000.0); // mTicksPerSecond = 1000 => ticks are in milliseconds
	m_duration = static_cast<float>(animation->mDuration); // duration in ticks
	m_durationInSeconds = m_duration / m_ticksPerSecond;
	m_desiredFPS = computeFPSFromAssimp(animation); // usually 30 FPS for mixamo animations
	m_numFrames = static_cast<unsigned int>(m_durationInSeconds * m_desiredFPS);

	aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
	globalTransformation = globalTransformation.Inverse();

	// Build hierarchy from first root node
	readHierarchyDataFromAssimp(m_rootNode, scene->mRootNode);
	
	// Bones
	readMissingBonesFromAssimp(animation, *model.get());
}

void engine::BoneAnimation::importBoneAnimationFromGLTF(
	const std::string& animationPath,
	std::shared_ptr<Model> model)
{
	tinygltf3::Model gltfModel;

	tg3_parse_options opts{};
	tg3_parse_options_init(&opts);
	opts.images_as_is = 0;

	tg3_error_stack errors{};
	tg3_error_stack_init(&errors);

	tg3_error_code err = tg3_parse_file(
		&gltfModel.raw(),
		&errors,
		animationPath.c_str(),
		(uint32_t)animationPath.size(),
		&opts
	);

	if (err != TG3_OK)
	{
		logger.error("GLTF loading error: {}", animationPath);
		for (uint32_t i = 0; i < errors.count; i++)
		{
			fprintf(stderr, "[%d] %s\n",
				(int)errors.entries[i].severity,
				errors.entries[i].message ? errors.entries[i].message : "(null)");
		}
		return;
	}

	const tg3_model& raw = gltfModel.raw();

	if (raw.animations_count == 0)
	{
		logger.error("GLTF contains no animations: {}", animationPath);
		return;
	}

	// For now: use animation #0
	const tg3_animation& animation = raw.animations[0];

	// --- Timing ---
	m_ticksPerSecond = 1000; // we’ll convert seconds → ms
	float durationSeconds = computeDurationFromGLTF(raw, animation);
	m_duration = durationSeconds * m_ticksPerSecond;
	m_durationInSeconds = durationSeconds;

	m_desiredFPS = computeFPSFromGLTF(raw, animation);
	if (m_desiredFPS == 0)
		m_desiredFPS = 30; // fallback

	m_numFrames = static_cast<unsigned int>(m_durationInSeconds * m_desiredFPS);

	// --- Skeleton / hierarchy ---
	if (raw.skins_count == 0)
	{
		logger.error("GLTF has animation but no skins: {}", animationPath);
		return;
	}

	// For now: use skin #0 (matching your mesh)
	const tg3_skin& skin = raw.skins[0];

	// Determine skeleton root
	//uint32_t skeletonRootIndex;
	//if (skin.skeleton != UINT32_MAX)
	//{
	//	skeletonRootIndex = skin.skeleton;
	//}
	//else
	//{
	//	// If no explicit skeleton root, use first joint as root
	//	skeletonRootIndex = skin.joints[0];
	//}

	//const tg3_node& skeletonRootNode = raw.nodes[skeletonRootIndex];
	//readHierarchyDataFromGLTF(m_rootNode, raw, skeletonRootNode);

	//// --- Bones / animation channels ---
	//readMissingBonesFromGLTF(raw, animation, *model.get());

	// Try to find an "Armature" node to match Assimp
	int armatureNodeIndex = -1;
	for (uint32_t i = 0; i < raw.nodes_count; ++i)
	{
		std::string nodeName(raw.nodes[i].name.data, raw.nodes[i].name.len);
		if (nodeName == "Armature")
		{
			armatureNodeIndex = (int)i;
			break;
		}
	}

	uint32_t skeletonRootIndex;
	if (armatureNodeIndex >= 0)
	{
		// Match Assimp: root = Armature
		skeletonRootIndex = (uint32_t)armatureNodeIndex;
	}
	else
	{
		// Fallback: use skin.skeleton or first joint
		if (skin.skeleton != UINT32_MAX)
			skeletonRootIndex = skin.skeleton;
		else
			skeletonRootIndex = skin.joints[0];
	}

	const tg3_node& skeletonRootNode = raw.nodes[skeletonRootIndex];
	readHierarchyDataFromGLTF(m_rootNode, raw, skeletonRootNode);

	// --- Bones / animation channels ---
	readMissingBonesFromGLTF(raw, animation, *model.get());
}



engine::Bone* engine::BoneAnimation::findBone(const std::string& name)
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


engine::Bone engine::BoneAnimation::createBone(const std::string& name,	int id,	std::vector<KeyPosition> positions,	std::vector<KeyRotation> rotations,	std::vector<KeyScale> scales)
{
	return Bone(name, id,
		std::move(positions),
		std::move(rotations),
		std::move(scales));
}

void engine::BoneAnimation::readMissingBonesFromAssimp(const aiAnimation* animation, Model& model)
{
	auto& boneInfoMap = model.getBoneInfoMap();
	int boneCount = model.getBoneCount();

	for (unsigned i = 0; i < animation->mNumChannels; i++)
	{
		const aiNodeAnim* channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (boneInfoMap.find(boneName) == boneInfoMap.end())
			boneInfoMap[boneName].id = boneCount++;

		std::vector<KeyPosition> positions;
		std::vector<KeyRotation> rotations;
		std::vector<KeyScale> scales;

		extractBoneKeysFromAssimp(channel, positions, rotations, scales);

		Bone bone = createBone(boneName, boneInfoMap[boneName].id, positions, rotations, scales);
		m_bones.push_back(bone);
	}

	m_boneInfoMap = boneInfoMap;
}

void engine::BoneAnimation::readMissingBonesFromGLTF(
	const tg3_model& gltfModel,
	const tg3_animation& animation,
	Model& engineModel)
{
	auto& boneInfoMap = engineModel.getBoneInfoMap();
	int boneCount = engineModel.getBoneCount();

	struct BoneChannelData {
		std::vector<KeyPosition> positions;
		std::vector<KeyRotation> rotations;
		std::vector<KeyScale>    scales;
	};

	std::unordered_map<int, BoneChannelData> channelMap;

	// Group channels by node index
	for (uint32_t i = 0; i < animation.channels_count; i++)
	{
		const tg3_animation_channel& channel = animation.channels[i];
		int nodeIndex = channel.target.node;

		BoneChannelData& data = channelMap[nodeIndex];

		extractBoneKeysFromGltf(
			gltfModel,
			animation,
			channel,
			data.positions,
			data.rotations,
			data.scales
		);
	}

	// Create one Bone per animated node
	for (auto& [nodeIndex, data] : channelMap)
	{
		const tg3_node& node = gltfModel.nodes[nodeIndex];
		std::string boneName = node.name.data;

		// If your Assimp path uses different names (e.g. "mixamorig:LeftArm"),
		// you may need a remapping table here.

		//if (boneInfoMap.find(boneName) == boneInfoMap.end())
		//	boneInfoMap[boneName].id = boneCount++;

		if (boneInfoMap.find(boneName) == boneInfoMap.end())
			continue; // skip channels for nodes that are not skinned



		int id = boneInfoMap[boneName].id;

		Bone bone = createBone(
			boneName,
			id,
			data.positions,
			data.rotations,
			data.scales
		);

		m_bones.push_back(bone);
	}

	m_boneInfoMap = boneInfoMap;
}



void engine::BoneAnimation::extractBoneKeysFromAssimp(const aiNodeAnim* channel, std::vector<KeyPosition>& positions, std::vector<KeyRotation>& rotations, std::vector<KeyScale>& scales)
{
	for (unsigned i = 0; i < channel->mNumPositionKeys; ++i)
		positions.push_back({
			AssimpGLMHelpers::GetGLMVec(channel->mPositionKeys[i].mValue),
			float(channel->mPositionKeys[i].mTime)
			});

	for (unsigned i = 0; i < channel->mNumRotationKeys; ++i)
		rotations.push_back({
			AssimpGLMHelpers::GetGLMQuat(channel->mRotationKeys[i].mValue),
			float(channel->mRotationKeys[i].mTime)
			});

	for (unsigned i = 0; i < channel->mNumScalingKeys; ++i)
		scales.push_back({
			AssimpGLMHelpers::GetGLMVec(channel->mScalingKeys[i].mValue),
			float(channel->mScalingKeys[i].mTime)
			});
}

void engine::BoneAnimation::extractBoneKeysFromGltf(
	const tg3_model& model,
	const tg3_animation& anim,
	const tg3_animation_channel& channel,
	std::vector<KeyPosition>& positions,
	std::vector<KeyRotation>& rotations,
	std::vector<KeyScale>& scales)
{
	const tg3_animation_sampler& sampler = anim.samplers[channel.sampler];

	const tg3_accessor& inputAcc = model.accessors[sampler.input];
	const tg3_accessor& outputAcc = model.accessors[sampler.output];

	const tg3_buffer_view& inputBV = model.buffer_views[inputAcc.buffer_view];
	const tg3_buffer_view& outputBV = model.buffer_views[outputAcc.buffer_view];

	const tg3_buffer& inputBuf = model.buffers[inputBV.buffer];
	const tg3_buffer& outputBuf = model.buffers[outputBV.buffer];

	const float* times = reinterpret_cast<const float*>(
		inputBuf.data.data +
		inputBV.byte_offset +
		inputAcc.byte_offset);

	const float* values = reinterpret_cast<const float*>(
		outputBuf.data.data +
		outputBV.byte_offset +
		outputAcc.byte_offset);

	for (uint32_t i = 0; i < inputAcc.count; ++i)
	{
		//float t = times[i];
		float t = times[i] * 1000.0f;

		if (channel.target.path.data == "translation")
		{
			glm::vec3 pos(values[i * 3 + 0], values[i * 3 + 1], values[i * 3 + 2]);
			positions.push_back({ pos, t });
		}
		else if (channel.target.path.data == "rotation")
		{
			glm::quat rot(values[i * 4 + 3], values[i * 4 + 0], values[i * 4 + 1], values[i * 4 + 2]);
			rotations.push_back({ rot, t });
		}
		else if (channel.target.path.data == "scale")
		{
			glm::vec3 scl(values[i * 3 + 0], values[i * 3 + 1], values[i * 3 + 2]);
			scales.push_back({ scl, t });
		}
	}

	// If translation is missing, use node's default translation
	if (positions.empty())
	{
		const tg3_node& node = model.nodes[channel.target.node];

		glm::vec3 pos(node.translation[0],
			node.translation[1],
			node.translation[2]);

		positions.push_back({ pos, 0.0f });
	}

	// If rotation is missing, use node's default rotation
	if (rotations.empty())
	{
		const tg3_node& node = model.nodes[channel.target.node];

		glm::quat rot(node.rotation[3], // w
			node.rotation[0], // x
			node.rotation[1], // y
			node.rotation[2]);// z

		rotations.push_back({ rot, 0.0f });
	}

	// If scale is missing, use node's default scale
	if (scales.empty())
	{
		const tg3_node& node = model.nodes[channel.target.node];

		glm::vec3 scl(node.scale[0],
			node.scale[1],
			node.scale[2]);

		scales.push_back({ scl, 0.0f });
	}



	// Fill missing rotation/scale with identity
	//if (rotations.empty())
	//{
	//	rotations.resize(positions.size());
	//	for (size_t i = 0; i < positions.size(); ++i)
	//		rotations[i] = { glm::quat(1,0,0,0), positions[i].timeStamp };
	//}

	//if (scales.empty())
	//{
	//	scales.resize(positions.size());
	//	for (size_t i = 0; i < positions.size(); ++i)
	//		scales[i] = { glm::vec3(1,1,1), positions[i].timeStamp };
	//}
}




void engine::BoneAnimation::readHierarchyDataFromAssimp(AnimNodeData& dest, const aiNode* src)
{
	assert(src);

	dest.name = src->mName.data;
	dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
	dest.childrenCount = src->mNumChildren;

	for (unsigned int i = 0; i < src->mNumChildren; i++)
	{
		AnimNodeData newData;
		readHierarchyDataFromAssimp(newData, src->mChildren[i]);
		dest.children.push_back(newData);
	}
}

void engine::BoneAnimation::readHierarchyDataFromGLTF(
	AnimNodeData& dest,
	const tg3_model& model,
	const tg3_node& src)
{
	dest.name = src.name.data;

	glm::mat4 transform(1.0f);

	if (src.has_matrix == 1)
	{
		// GLTF matrices are column-major; tinygltf3 gives float[16] in column-major
		transform = glm::make_mat4x4(src.matrix);
	}
	else
	{
		glm::vec3 translation(src.translation[0],
			src.translation[1],
			src.translation[2]);

		glm::vec3 scale(src.scale[0],
			src.scale[1],
			src.scale[2]);

		glm::quat rotation(src.rotation[3], // w
			src.rotation[0], // x
			src.rotation[1], // y
			src.rotation[2]);// z

		transform =
			glm::translate(glm::mat4(1.0f), translation) *
			glm::toMat4(rotation) *
			glm::scale(glm::mat4(1.0f), scale);
	}

	dest.transformation = transform;

	dest.childrenCount = src.children_count;

	for (uint32_t i = 0; i < src.children_count; i++)
	{
		uint32_t childIndex = src.children[i];
		const tg3_node& childNode = model.nodes[childIndex];

		AnimNodeData childData;
		readHierarchyDataFromGLTF(childData, model, childNode);
		dest.children.push_back(childData);
	}
}



// ensure anim was exported at 30 FPS (should be always the case for mixamo anims)
// should be 0, 33.3333, 66.6667, 100...
// 33.33 ms = 1/30s
unsigned int engine::BoneAnimation::computeFPSFromAssimp(const aiAnimation* anim)
{
	if (anim->mNumChannels == 0)
		return 0;

	const aiNodeAnim* channel = anim->mChannels[0];

	if (channel->mNumPositionKeys < 2)
		return 0;

	double t0 = channel->mPositionKeys[0].mTime;
	double t1 = channel->mPositionKeys[1].mTime;

	double deltaTicks = t1 - t0;

	double ticksPerSecond = anim->mTicksPerSecond;
	if (ticksPerSecond <= 0.0)
		ticksPerSecond = 1000.0; // Assimp fallback for FBX

	double deltaSeconds = deltaTicks / ticksPerSecond;


	//const aiNodeAnim* channel = animation->mChannels[0];
	//for (int i = 0; i < channel->mNumPositionKeys; i++) {
	//	std::cout << channel->mPositionKeys[i].mTime << std::endl;
	//}

	return static_cast<unsigned int>(std::round(1.0 / deltaSeconds));
}

unsigned int engine::BoneAnimation::computeFPSFromGLTF(const tg3_model& model, const tg3_animation& anim)
{
	if (anim.channels_count == 0)
		return 0;

	const tg3_animation_channel& channel = anim.channels[0];
	const tg3_animation_sampler& sampler = anim.samplers[channel.sampler];

	const tg3_accessor& inputAcc = model.accessors[sampler.input];
	const tg3_buffer_view& inputBV = model.buffer_views[inputAcc.buffer_view];
	const tg3_buffer& inputBuf = model.buffers[inputBV.buffer];

	const float* times = reinterpret_cast<const float*>(
		inputBuf.data.data +
		inputBV.byte_offset +
		inputAcc.byte_offset);

	if (inputAcc.count < 2)
		return 0;

	float delta = times[1] - times[0];
	if (delta <= 0.0f)
		return 0;

	return static_cast<unsigned int>(std::round(1.0f / delta));
}


float engine::BoneAnimation::computeDurationFromGLTF(const tg3_model& model, const tg3_animation& anim)
{
	float minTime = std::numeric_limits<float>::max();
	float maxTime = std::numeric_limits<float>::lowest();

	for (uint32_t c = 0; c < anim.channels_count; c++)
	{
		const tg3_animation_channel& channel = anim.channels[c];
		const tg3_animation_sampler& sampler = anim.samplers[channel.sampler];

		const tg3_accessor& inputAcc = model.accessors[sampler.input];
		const tg3_buffer_view& inputBV = model.buffer_views[inputAcc.buffer_view];
		const tg3_buffer& inputBuf = model.buffers[inputBV.buffer];

		const float* times = reinterpret_cast<const float*>(
			inputBuf.data.data +
			inputBV.byte_offset +
			inputAcc.byte_offset);

		for (uint32_t i = 0; i < inputAcc.count; i++)
		{
			float t = times[i];
			minTime = std::min(minTime, t);
			maxTime = std::max(maxTime, t);
		}
	}

	if (maxTime < minTime)
		return 0.0f;

	return maxTime - minTime;
}


engine::BoneAnimation::~BoneAnimation()
{
	logger.trace("BoneAnimation destructor called");
}
