#pragma once

#include <vector>
#include <map>
#include <functional>

#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

#include "model.h"
#include "bone.h"
#include "assimp_glm_helpers.h"


namespace engine
{
	struct AssimpNodeData
	{
		glm::mat4 transformation{};
		std::string name{};
		int childrenCount{};
		std::vector<AssimpNodeData> children{};
	};

	class Animation
	{
	public:
		Animation() = default;
		Animation(const std::string& animationPath, std::shared_ptr<Model> model, float speed = 1.0f);

		~Animation() = default;

		Bone* findBone(const std::string& name);


		inline int getTicksPerSecond() { return m_ticksPerSecond; }
		inline float getDuration() { return m_duration; }
		inline const AssimpNodeData& getRootNode() { return m_rootNode; }
		inline const std::map<std::string, BoneInfo>& getBoneIDMap() { return m_boneInfoMap; }

	private:
		float m_duration{};
		int m_ticksPerSecond{};
		std::vector<Bone> m_bones{};
		AssimpNodeData m_rootNode{};
		std::map<std::string, BoneInfo> m_boneInfoMap{};
		
		void readMissingBones(const aiAnimation* animation, Model& model);
		void readHierarchyData(AssimpNodeData& dest, const aiNode* src);
	};
}