#include "../../include/animations/bone_animation.h"

#include "../../include/managers/log_manager.h"


#include <assimp/importer.hpp>
#include <assimp/postprocess.h>

#include <glm/gtc/type_ptr.hpp>


engine::BoneAnimation::BoneAnimation(const std::string& animationName, const std::string& animationPath, std::shared_ptr<Model> model, float speedFactor)
	: Animation(animationName, model, speedFactor), m_filepath(animationPath)
{
	logger.trace("BoneAnimation constructor called");

	importBoneAnimationFromAssimp(animationPath, model);
	
	//importBoneAnimationFromGLTF(animationPath, model);
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

void engine::BoneAnimation::importBoneAnimationFromGLTF(const std::string& animationPath, std::shared_ptr<Model> model)
{
	// --- Load GLTF file ---
	tinygltf3::Model gltfModel;

	tg3_parse_options opts{};
	tg3_parse_options_init(&opts);
	opts.images_as_is = 0; // keep raw bytes

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
		// handle errors
		logger.error("GLTF loading error: unknown");

		for (uint32_t i = 0; i < errors.count; i++) {
			fprintf(stderr, "[%d] %s\n", (int)errors.entries[i].severity,
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

	// For now: use animation #0 (same as Assimp path)
	const tg3_animation& animation = raw.animations[0];

	// --- Timing ---
	// GLTF animations use seconds, not ticks.
	// tinyGLTF3 stores input accessor values directly as seconds.
	m_ticksPerSecond = 1; // seconds → no conversion needed
	m_duration = computeDurationFromGLTF(raw, animation) * 1000; // seconds // 9466.66602f;
	m_durationInSeconds = m_duration / 1000.0f; // 9.46666622f

	// Compute FPS from GLTF (same logic as Assimp but using GLTF channel)
	m_desiredFPS = 30; // TO DO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! computeFPSFromGLTF(gltfModel, animation);
	m_numFrames = static_cast<unsigned int>(m_durationInSeconds * m_desiredFPS); // 284



	// --- Hierarchy ---
	// GLTF has multiple scene roots; use scene 0
	const tg3_scene& scene = raw.scenes[raw.default_scene]; // ?????????
	if (scene.nodes_count == 0)
	{
		logger.error("GLTF scene has no root nodes");
		return;
	}

	// Build hierarchy from first root node
	uint32_t rootIndex = scene.nodes[0];
	const tg3_node& rootNode = raw.nodes[rootIndex];
	readHierarchyDataFromGLTF(m_rootNode, raw, rootNode);

	// Bones
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

void engine::BoneAnimation::readMissingBonesFromGLTF(const tg3_model& gltfModel, const tg3_animation& animation, Model& engineModel)
{
	auto& boneInfoMap = engineModel.getBoneInfoMap();
	int boneCount = engineModel.getBoneCount();

	// --- Step 1: group channels by node index ---
	struct BoneChannelData {
		std::vector<KeyPosition> positions;
		std::vector<KeyRotation> rotations;
		std::vector<KeyScale>    scales;
	};

	std::unordered_map<int, BoneChannelData> channelMap;

	for (uint32_t i = 0; i < animation.channels_count; i++)
	{
		const tg3_animation_channel& channel = animation.channels[i];
		int nodeIndex = channel.target.node;

		BoneChannelData& data = channelMap[nodeIndex];

		// Fill only the relevant TRS component
		extractBoneKeysFromGltf(
			gltfModel,
			animation,
			channel,
			data.positions,
			data.rotations,
			data.scales
		);
	}

	// --- Step 2: create one Bone per node ---
	for (auto& [nodeIndex, data] : channelMap)
	{
		const tg3_node& node = gltfModel.nodes[nodeIndex];
		std::string boneName = node.name.data;

		// Register bone if missing
		if (boneInfoMap.find(boneName) == boneInfoMap.end())
			boneInfoMap[boneName].id = boneCount++;

		Bone bone = createBone(
			boneName,
			boneInfoMap[boneName].id,
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

void engine::BoneAnimation::extractBoneKeysFromGltf(const tg3_model& model,	const tg3_animation& anim, const tg3_animation_channel& channel, std::vector<KeyPosition>& positions, std::vector<KeyRotation>& rotations, std::vector<KeyScale>& scales)
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
		float t = times[i];

		if (channel.target.path.data == std::string("translation"))
		{
			glm::vec3 pos(values[i * 3 + 0], values[i * 3 + 1], values[i * 3 + 2]);
			positions.push_back({ pos, t });
		}
		else if (channel.target.path.data == std::string("rotation"))
		{
			glm::quat rot(values[i * 4 + 3], values[i * 4 + 0],
				values[i * 4 + 1], values[i * 4 + 2]);
			rotations.push_back({ rot, t });
		}
		else if (channel.target.path.data == std::string("scale"))
		{
			glm::vec3 scl(values[i * 3 + 0], values[i * 3 + 1], values[i * 3 + 2]);
			scales.push_back({ scl, t });
		}
	}

	if (rotations.empty()) {
		rotations.resize(positions.size());
		for (size_t i = 0; i < positions.size(); ++i)
			rotations[i] = { glm::quat(1,0,0,0), positions[i].timeStamp };
	}

	if (scales.empty()) {
		scales.resize(positions.size());
		for (size_t i = 0; i < positions.size(); ++i)
			scales[i] = { glm::vec3(1,1,1), positions[i].timeStamp };
	}

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

void engine::BoneAnimation::readHierarchyDataFromGLTF(AnimNodeData& dest, const tg3_model& model, const tg3_node& src)
{
	dest.name = src.name.data;

	glm::mat4 transform(1.0f);

	// If matrix is explicitly set, use it
	if (src.has_matrix == 1)
	{
		transform = glm::make_mat4x4(src.matrix);
	}
	else
	{
		// Otherwise use TRS (always valid, defaults applied by tinygltf3)
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

	// Children
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
