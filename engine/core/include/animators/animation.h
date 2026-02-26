#pragma once

#include <vector>
#include <map>
#include <functional>

#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

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

	class Animation : private NonCopyable
	{
	public:
		Animation() = default;
		Animation(const std::string& animationName, const std::string& animationPath, std::shared_ptr<Model> model, float speedFactor = 1.0f);

		~Animation() = default;

		Bone* findBone(const std::string& name);


		inline int getTicksPerSecond() { return m_ticksPerSecond; }
		inline float getDuration() { return m_duration; }
		inline float getDurationInSeconds() { return m_durationInSeconds; }
		inline unsigned int getFramesCount() { return m_numFrames; }
		inline unsigned int getFPS() { return m_desiredFPS; }
		inline float getSpeedFactor() const { return m_speedFactor; }
		inline const AssimpNodeData& getRootNode() { return m_rootNode; }
		inline const std::map<std::string, BoneInfo>& getBoneIDMap() { return m_boneInfoMap; }

		std::string& getName() { return m_name; }
		void setName(std::string name) { m_name = name; }

		float& getSpeedFactor() { return m_speedFactor; }
		void setSpeedFactor(float speedFactor) { m_speedFactor = speedFactor; }

		std::string getFilepath() { return m_filepath; }

		size_t getBoneCount() { return m_bones.size(); }
		


	private:
		std::string m_name{};
		std::string m_filepath{};
		float m_speedFactor{};
		float m_duration{};
		float m_durationInSeconds{};
		int m_ticksPerSecond{};
		unsigned int m_desiredFPS{30u};
		unsigned int m_numFrames{};
		std::vector<Bone> m_bones{};
		//std::unordered_map<std::string, Bone> m_BoneKeys;
		AssimpNodeData m_rootNode{};
		std::map<std::string, BoneInfo> m_boneInfoMap{};
		
		void readMissingBones(const aiAnimation* animation, Model& model);
		void readHierarchyData(AssimpNodeData& dest, const aiNode* src);
	};
}