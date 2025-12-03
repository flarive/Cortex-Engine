#pragma once

#include <vector>
#include <map>
#include <functional>

#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

#include "model.h"
#include "bone.h"
#include "animdata.h"


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

		Animation(const std::string& animationPath, std::shared_ptr<Model> model)
		{
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
			assert(scene && scene->mRootNode);
			auto animation = scene->mAnimations[0];
			m_duration = static_cast<float>(animation->mDuration);
			m_ticksPerSecond = static_cast<int>(animation->mTicksPerSecond);
			aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
			globalTransformation = globalTransformation.Inverse();
			readHierarchyData(m_rootNode, scene->mRootNode);
			readMissingBones(animation, *model.get());
		}

		~Animation()
		{
		}

		Bone* findBone(const std::string& name)
		{
			auto iter = std::find_if(m_bones.begin(), m_bones.end(),
				[&](const Bone& Bone)
				{
					return Bone.getBoneName() == name;
				}
			);
			if (iter == m_bones.end()) return nullptr;
			else return &(*iter);
		}


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
		
		void readMissingBones(const aiAnimation* animation, Model& model)
		{
			int size = animation->mNumChannels;

			auto& boneInfoMap = model.getBoneInfoMap();//getting m_boneInfoMap from Model class
			int& boneCount = model.getBoneCount(); //getting the m_boneCounter from Model class

			//reading channels(bones engaged in an animation and their keyframes)
			for (int i = 0; i < size; i++)
			{
				auto channel = animation->mChannels[i];
				std::string boneName = channel->mNodeName.data;

				if (boneInfoMap.find(boneName) == boneInfoMap.end())
				{
					boneInfoMap[boneName].id = boneCount;
					boneCount++;
				}
				m_bones.push_back(Bone(channel->mNodeName.data,
					boneInfoMap[channel->mNodeName.data].id, channel));
			}

			m_boneInfoMap = boneInfoMap;
		}

		void readHierarchyData(AssimpNodeData& dest, const aiNode* src)
		{
			assert(src);

			dest.name = src->mName.data;
			dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
			dest.childrenCount = src->mNumChildren;

			for (unsigned int i = 0; i < src->mNumChildren; i++)
			{
				AssimpNodeData newData;
				readHierarchyData(newData, src->mChildren[i]);
				dest.children.push_back(newData);
			}
		}
	};
}