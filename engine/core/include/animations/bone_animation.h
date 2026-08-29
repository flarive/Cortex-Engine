#pragma once

#include <vector>
#include <map>
#include <functional>

#include <assimp/scene.h>
#include <assimp/matrix4x4.h>
#include "tiny_gltf_v3.h"


#include "animation.h"
#include "../models/model.h"
#include "../models/bone.h"
#include "../models/assimp_glm_helpers.h"


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
		std::vector<KeyScale>    scales;
	};


	class BoneAnimation final : public Animation
	{
	public:
		BoneAnimation() = default;
		BoneAnimation(const std::string& animationName, const std::string& animationPath, std::shared_ptr<Model> model, float speedFactor = 1.0f);

		~BoneAnimation() override;

		AnimationType getTypeID() const override
		{
			return AnimationType::bones;
		}


		inline std::string& getFilepath() { return m_filepath; }
		void setFilepath(const std::string& filepath) { m_filepath = filepath; }

		Bone* findBone(const std::string& name);
		const AnimNodeData& getRootNode() { return m_rootNode; }
		const std::map<std::string, BoneInfo>& getBoneIDMap() { return m_boneInfoMap; }
		size_t getBoneCount() { return m_bones.size(); }
		


	private:
		std::string m_filepath{};
		std::vector<Bone> m_bones{};
		AnimNodeData m_rootNode{};
		std::map<std::string, BoneInfo> m_boneInfoMap{};

		void importBoneAnimationFromAssimp(const std::string& animationPath, std::shared_ptr<Model> model);
		void importBoneAnimationFromGLTF(const std::string& animationPath, std::shared_ptr<Model> model);
		
		void readMissingBonesFromAssimp(const aiAnimation* animation, Model& model);
		void readHierarchyDataFromAssimp(AnimNodeData& dest, const aiNode* src);

		void readMissingBonesFromGLTF(const tg3_model& gltfModel, const tg3_animation& animation, Model& engineModel);
		void readHierarchyDataFromGLTF(AnimNodeData& dest, const tg3_model& model, const tg3_node& src);

		unsigned int computeFPSFromAssimp(const aiAnimation* anim);
		unsigned int computeFPSFromGLTF(const tg3_model& model, const tg3_animation& anim);

		float computeDurationFromGLTF(const tg3_model& model, const tg3_animation& anim);

		Bone createBone(const std::string& name, int id, std::vector<KeyPosition> positions, std::vector<KeyRotation> rotations, std::vector<KeyScale> scales);
		
		void extractBoneKeysFromAssimp(const aiNodeAnim* channel, std::vector<KeyPosition>& positions, std::vector<KeyRotation>& rotations, std::vector<KeyScale>& scales);
		void extractBoneKeysFromGltf(const tg3_model& model, const tg3_animation& anim,	const tg3_animation_channel& channel, std::vector<KeyPosition>& positions, std::vector<KeyRotation>& rotations,	std::vector<KeyScale>& scales);
	};
}