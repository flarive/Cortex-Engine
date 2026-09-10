#pragma once

#include "../../misc/noncopyable.h"
#include "../../common_defines.h"

#include "../../models/model.h"
#include "../../models/bone.h"

namespace engine
{
	struct AnimNodeData final
	{
		glm::mat4 transformation{};
		std::string name{};
		int childrenCount{};
		std::vector<AnimNodeData> children{};
	};

	struct BoneChannelData final
	{
		std::vector<KeyPosition> positions;
		std::vector<KeyRotation> rotations;
		std::vector<KeyScale> scales;
	};


	/// <summary>
	/// Abstract class for animations loader
	/// </summary>
	class AnimationLoader : public NonCopyable
	{
	public:
		AnimationLoader();
		virtual ~AnimationLoader();

		static std::unique_ptr<AnimationLoader> create(const std::string& path);


		virtual void loadAnimation(const std::string& animationPath, std::shared_ptr<Model> model) = 0;

		engine::Bone* findBone(const std::string& name);

		const std::vector<Bone>& getBones() const { return m_bones; }
		const unsigned int getBoneCount() const { return static_cast<unsigned int>(m_bones.size()); }

		const AnimNodeData& getRootNode() const { return m_rootNode; }




		float getDuration() const { return m_duration; }
		float getDurationInSeconds() const { return m_durationInSeconds; }
		unsigned int getTicksPerSecond() const { return m_ticksPerSecond; }
		unsigned int getDesiredFPS() const { return m_desiredFPS; }
		unsigned int getNumFrames() const { return m_numFrames; }

		int findSkeletonBoneIndex(const Model& model, const std::string& name);
	

	protected:
		std::vector<Bone> m_bones{}; // animated bones, not the same as SkeletonBones
		AnimNodeData m_rootNode{};

		float m_duration{};
		float m_durationInSeconds{};
		unsigned int m_ticksPerSecond{};
		unsigned int m_desiredFPS{ 30u };
		unsigned int m_numFrames{};
		
		engine::Bone createBone(const std::string& name, int id, std::vector<KeyPosition> positions, std::vector<KeyRotation> rotations, std::vector<KeyScale> scales);
	};
}