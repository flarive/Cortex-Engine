#include "../../../include/animations/loaders/tinygltf_animation_loader.h"

#include "../../../include/managers/log_manager.h"

#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>
#include <limits>
#include <cmath>


engine::TinygltfAnimationLoader::TinygltfAnimationLoader()
{
    logger.trace("TinygltfAnimationLoader constructor called");
}

void engine::TinygltfAnimationLoader::loadAnimation(const std::string& animationPath,
    std::shared_ptr<Model> model)
{
    importBoneAnimation(animationPath, model);
}

void engine::TinygltfAnimationLoader::importBoneAnimation(const std::string& animationPath, std::shared_ptr<Model> model)
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
        &opts);

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

    // For now: use animation #0 (Mixamo usually exports one per file)
    const tg3_animation& animation = raw.animations[0];

    // --- Timing (compatible with BonesAnimator) ---
    // GLTF times are in seconds; we treat "ticks" as milliseconds.
    m_ticksPerSecond = 1000; // ticks = ms
    float durationSec = computeDuration(raw, animation);
    m_durationInSeconds = durationSec;
    m_duration = durationSec * m_ticksPerSecond;

    m_desiredFPS = computeFPS(raw, animation);
    if (m_desiredFPS == 0)
        m_desiredFPS = 30; // Mixamo default

    m_numFrames = static_cast<unsigned int>(m_durationInSeconds * m_desiredFPS);

    // --- Skeleton / hierarchy ---
    if (raw.skins_count == 0)
    {
        logger.error("GLTF has animation but no skins: {}", animationPath);
        return;
    }

    const tg3_skin& skin = raw.skins[0];

    uint32_t skeletonRootIndex = UINT32_MAX;

    // Try to find an "Armature" node (Mixamo convention)
    int armatureNodeIndex = -1;
    for (uint32_t i = 0; i < raw.nodes_count; ++i)
    {
        const tg3_node& n = raw.nodes[i];
        std::string nodeName(n.name.data, n.name.len);
        if (nodeName == "Armature")
        {
            armatureNodeIndex = (int)i;
            break;
        }
    }

    if (armatureNodeIndex >= 0)
    {
        skeletonRootIndex = (uint32_t)armatureNodeIndex;
    }
    else
    {
        if (skin.skeleton != UINT32_MAX)
            skeletonRootIndex = skin.skeleton;
        else
            skeletonRootIndex = skin.joints[0];
    }

    const tg3_node& skeletonRootNode = raw.nodes[skeletonRootIndex];

    // Build hierarchy (like AssimpAnimationLoader::readHierarchyData)
    m_rootNode = AnimNodeData{};
    readHierarchyData(m_rootNode, raw, skeletonRootNode);

    // --- Bones (same pattern as AssimpAnimationLoader::readMissingBones) ---
    readMissingBones(raw, animation, *model.get());
}

void engine::TinygltfAnimationLoader::readMissingBones(const tg3_model& gltfModel, const tg3_animation& animation, Model& model)
{
    auto& boneInfoMap = model.getBoneInfoMap();
    int   boneCount = model.getBoneCount();

    struct BoneChannelData
    {
        std::vector<KeyPosition> positions;
        std::vector<KeyRotation> rotations;
        std::vector<KeyScale>    scales;
    };

    std::unordered_map<int, BoneChannelData> channelMap;

    // Group channels by node index
    for (uint32_t i = 0; i < animation.channels_count; ++i)
    {
        const tg3_animation_channel& channel = animation.channels[i];
        int nodeIndex = channel.target.node;

        BoneChannelData& data = channelMap[nodeIndex];
        extractBoneKeys(gltfModel, animation, channel,
            data.positions, data.rotations, data.scales);
    }

    m_bones.clear();

    // Create one Bone per animated node
    for (auto& [nodeIndex, data] : channelMap)
    {
        const tg3_node& node = gltfModel.nodes[nodeIndex];
        std::string boneName(node.name.data, node.name.len);

        // Mirror Assimp behavior: if bone not in map, assign new id
        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            boneInfoMap[boneName].id = boneCount++;
        }

        int id = boneInfoMap[boneName].id;

        Bone bone = createBone(
            boneName,
            id,
            data.positions,
            data.rotations,
            data.scales);

        m_bones.push_back(bone);
    }

    m_boneInfoMap = boneInfoMap;
}

void engine::TinygltfAnimationLoader::extractBoneKeys(
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

    std::string path(channel.target.path.data, channel.target.path.len);

    for (uint32_t i = 0; i < inputAcc.count; ++i)
    {
        // GLTF times are seconds; convert to "ticks" = ms to match BonesAnimator logic
        float t = times[i] * 1000.0f;

        if (path == "translation")
        {
            glm::vec3 pos(values[i * 3 + 0],
                values[i * 3 + 1],
                values[i * 3 + 2]);
            positions.push_back({ pos, t });
        }
        else if (path == "rotation")
        {
            glm::quat rot(values[i * 4 + 3], // w
                values[i * 4 + 0], // x
                values[i * 4 + 1], // y
                values[i * 4 + 2]);// z
            rotations.push_back({ rot, t });
        }
        else if (path == "scale")
        {
            glm::vec3 scl(values[i * 3 + 0],
                values[i * 3 + 1],
                values[i * 3 + 2]);
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
}

void engine::TinygltfAnimationLoader::readHierarchyData(AnimNodeData& dest, const tg3_model& model, const tg3_node& src)
{
    dest.name = std::string(src.name.data, src.name.len);

    glm::mat4 transform(1.0f);

    if (src.has_matrix == 1)
    {
        // GLTF matrices are column-major; tinygltf3 gives float[16] column-major
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

    dest.children.clear();
    dest.children.reserve(src.children_count);

    for (uint32_t i = 0; i < src.children_count; ++i)
    {
        uint32_t childIndex = src.children[i];
        const tg3_node& childNode = model.nodes[childIndex];

        AnimNodeData childData;
        readHierarchyData(childData, model, childNode);
        dest.children.push_back(childData);
    }
}

unsigned int engine::TinygltfAnimationLoader::computeFPS(const tg3_model& model, const tg3_animation& anim)
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

    // delta is in seconds; FPS = 1 / delta
    return static_cast<unsigned int>(std::round(1.0f / delta));
}

float engine::TinygltfAnimationLoader::computeDuration(const tg3_model& model, const tg3_animation& anim)
{
    float minTime = std::numeric_limits<float>::max();
    float maxTime = std::numeric_limits<float>::lowest();

    for (uint32_t c = 0; c < anim.channels_count; ++c)
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

        for (uint32_t i = 0; i < inputAcc.count; ++i)
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

engine::TinygltfAnimationLoader::~TinygltfAnimationLoader()
{
    logger.trace("TinygltfAnimationLoader destructor called");
}