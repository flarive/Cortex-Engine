#pragma once

/* Container for bone data */

#include <vector>
#include <list>

#include <assimp/scene.h>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "assimp_glm_helpers.h"

namespace engine
{
	struct KeyPosition
	{
		glm::vec3 position;
		float timeStamp;
	};

	struct KeyRotation
	{
		glm::quat orientation;
		float timeStamp;
	};

	struct KeyScale
	{
		glm::vec3 scale;
		float timeStamp;
	};

	class Bone
	{
	public:
		Bone(const std::string& name, int ID, const aiNodeAnim* channel)
			:
			m_name(name),
			m_ID(ID),
			m_localTransform(1.0f)
		{
			m_numPositions = channel->mNumPositionKeys;

			for (int positionIndex = 0; positionIndex < m_numPositions; ++positionIndex)
			{
				aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
				double timeStamp = channel->mPositionKeys[positionIndex].mTime;
				KeyPosition data;
				data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
				data.timeStamp = static_cast<float>(timeStamp);
				m_positions.push_back(data);
			}

			m_numRotations = channel->mNumRotationKeys;
			for (int rotationIndex = 0; rotationIndex < m_numRotations; ++rotationIndex)
			{
				aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
				double timeStamp = channel->mRotationKeys[rotationIndex].mTime;
				KeyRotation data;
				data.orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
				data.timeStamp = static_cast<float>(timeStamp);
				m_rotations.push_back(data);
			}

			m_numScalings = channel->mNumScalingKeys;
			for (int keyIndex = 0; keyIndex < m_numScalings; ++keyIndex)
			{
				aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
				double timeStamp = channel->mScalingKeys[keyIndex].mTime;
				KeyScale data;
				data.scale = AssimpGLMHelpers::GetGLMVec(scale);
				data.timeStamp = static_cast<float>(timeStamp);
				m_scales.push_back(data);
			}
		}

		void update(float animationTime)
		{
			glm::mat4 translation = interpolatePosition(animationTime);
			glm::mat4 rotation = interpolateRotation(animationTime);
			glm::mat4 scale = interpolateScaling(animationTime);
			m_localTransform = translation * rotation * scale;
		}
		glm::mat4 getLocalTransform() { return m_localTransform; }
		std::string getBoneName() const { return m_name; }
		int getBoneID() { return m_ID; }



		int getPositionIndex(float animationTime)
		{
			for (int index = 0; index < m_numPositions - 1; ++index)
			{
				if (animationTime < m_positions[index + 1].timeStamp)
					return index;
			}
			assert(0);

			return 0;
		}

		int getRotationIndex(float animationTime)
		{
			for (int index = 0; index < m_numRotations - 1; ++index)
			{
				if (animationTime < m_rotations[index + 1].timeStamp)
					return index;
			}
			assert(0);

			return 0;
		}

		int getScaleIndex(float animationTime)
		{
			for (int index = 0; index < m_numScalings - 1; ++index)
			{
				if (animationTime < m_scales[index + 1].timeStamp)
					return index;
			}
			assert(0);

			return 0;
		}


	private:

		std::vector<KeyPosition> m_positions{};
		std::vector<KeyRotation> m_rotations{};
		std::vector<KeyScale> m_scales{};
		int m_numPositions{};
		int m_numRotations{};
		int m_numScalings{};

		glm::mat4 m_localTransform{};
		std::string m_name{};
		int m_ID{};

		float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
		{
			float scaleFactor = 0.0f;
			float midWayLength = animationTime - lastTimeStamp;
			float framesDiff = nextTimeStamp - lastTimeStamp;
			scaleFactor = midWayLength / framesDiff;
			return scaleFactor;
		}

		glm::mat4 interpolatePosition(float animationTime)
		{
			if (1 == m_numPositions)
				return glm::translate(glm::mat4(1.0f), m_positions[0].position);

			int p0Index = getPositionIndex(animationTime);
			int p1Index = p0Index + 1;
			float scaleFactor = getScaleFactor(m_positions[p0Index].timeStamp, m_positions[p1Index].timeStamp, animationTime);
			glm::vec3 finalPosition = glm::mix(m_positions[p0Index].position, m_positions[p1Index].position, scaleFactor);
			return glm::translate(glm::mat4(1.0f), finalPosition);
		}

		glm::mat4 interpolateRotation(float animationTime)
		{
			if (1 == m_numRotations)
			{
				auto rotation = glm::normalize(m_rotations[0].orientation);
				return glm::toMat4(rotation);
			}

			int p0Index = getRotationIndex(animationTime);
			int p1Index = p0Index + 1;
			float scaleFactor = getScaleFactor(m_rotations[p0Index].timeStamp, m_rotations[p1Index].timeStamp, animationTime);
			glm::quat finalRotation = glm::slerp(m_rotations[p0Index].orientation, m_rotations[p1Index].orientation, scaleFactor);
			finalRotation = glm::normalize(finalRotation);
			return glm::toMat4(finalRotation);

		}

		glm::mat4 interpolateScaling(float animationTime)
		{
			if (1 == m_numScalings)
				return glm::scale(glm::mat4(1.0f), m_scales[0].scale);

			int p0Index = getScaleIndex(animationTime);
			int p1Index = p0Index + 1;
			float scaleFactor = getScaleFactor(m_scales[p0Index].timeStamp,	m_scales[p1Index].timeStamp, animationTime);
			glm::vec3 finalScale = glm::mix(m_scales[p0Index].scale, m_scales[p1Index].scale, scaleFactor);
			return glm::scale(glm::mat4(1.0f), finalScale);
		}
	};
}