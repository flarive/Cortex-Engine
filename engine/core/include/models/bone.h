#pragma once

/* Container for bone data */

#include <vector>
#include <list>

#include <assimp/scene.h>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>



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

	struct BoneInfo
	{
		/*id is index in finalBoneMatrices*/
		int id;

		/*offset matrix transforms vertex from model space to bone space*/
		glm::mat4 offset;
	};

	class Bone
	{
	public:
		Bone(const std::string& name, int ID, const aiNodeAnim* channel);

		void update(float animationTime);


		glm::mat4 getLocalTransform() { return m_localTransform; }
		std::string getBoneName() const { return m_name; }
		int getBoneID() { return m_ID; }



		int getPositionIndex(float animationTime);
		int getRotationIndex(float animationTime);
		int getScaleIndex(float animationTime);


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

		float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
		glm::mat4 interpolatePosition(float animationTime);
		glm::mat4 interpolateRotation(float animationTime);
		glm::mat4 interpolateScaling(float animationTime);
	};
}