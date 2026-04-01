#pragma once

#include <vector>
#include <map>
#include <functional>

#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

#include "animation.h"
#include "../models/model.h"
#include "../models/bone.h"
#include "../models/assimp_glm_helpers.h"


namespace engine
{
	struct AssimpNodeData
	{
		glm::mat4 transformation{};
		std::string name{};
		int childrenCount{};
		std::vector<AssimpNodeData> children{};
	};

	class BoneAnimation final : public Animation
	{
	public:
		BoneAnimation() = default;
		BoneAnimation(const std::string& animationName, const std::string& animationPath, std::shared_ptr<Model> model, float speedFactor = 1.0f);

		~BoneAnimation() = default;

		AnimationType getTypeID() const override
		{
			return AnimationType::bones;
		}


		inline std::string& getFilepath() { return m_filepath; }
		void setFilepath(const std::string& filepath) { m_filepath = filepath; }

		Bone* findBone(const std::string& name);
		const AssimpNodeData& getRootNode() { return m_rootNode; }
		const std::map<std::string, BoneInfo>& getBoneIDMap() { return m_boneInfoMap; }
		size_t getBoneCount() { return m_bones.size(); }
		


	private:
		std::string m_filepath{};
		std::vector<Bone> m_bones{};
		AssimpNodeData m_rootNode{};
		std::map<std::string, BoneInfo> m_boneInfoMap{};
		
		void readMissingBones(const aiAnimation* animation, Model& model);
		void readHierarchyData(AssimpNodeData& dest, const aiNode* src);

		unsigned int computeFPS(const aiAnimation* anim);
	};
}