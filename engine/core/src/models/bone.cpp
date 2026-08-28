#include "../../include/models/bone.h"

#include "../../include/models/assimp_glm_helpers.h"
#include "../../include/managers/log_manager.h"


engine::Bone::Bone(const std::string& name, int id, std::vector<KeyPosition> positions, std::vector<KeyRotation> rotations,	std::vector<KeyScale> scales)
	: m_name(name),	m_ID(id), m_positions(std::move(positions)), m_rotations(std::move(rotations)),	m_scales(std::move(scales)), m_localTransform(1.0f)
{
	m_numPositions = static_cast<int>(m_positions.size());
	m_numRotations = static_cast<int>(m_rotations.size());
	m_numScalings = static_cast<int>(m_scales.size());
}

void engine::Bone::update(float animationTime)
{
	glm::mat4 translation = interpolatePosition(animationTime);
	glm::mat4 rotation = interpolateRotation(animationTime);
	glm::mat4 scale = interpolateScaling(animationTime);
	m_localTransform = translation * rotation * scale;
}

int engine::Bone::getPositionIndex(float animationTime)
{
	for (int index = 0; index < m_numPositions - 1; ++index)
	{
		if (animationTime < m_positions[index + 1].timeStamp)
			return index;
	}
	assert(0);

	return 0;
}

int engine::Bone::getRotationIndex(float animationTime)
{
	for (int index = 0; index < m_numRotations - 1; ++index)
	{
		if (animationTime < m_rotations[index + 1].timeStamp)
			return index;
	}
	assert(0);

	return 0;
}

int engine::Bone::getScaleIndex(float animationTime)
{
	for (int index = 0; index < m_numScalings - 1; ++index)
	{
		if (animationTime < m_scales[index + 1].timeStamp)
			return index;
	}
	assert(0);

	return 0;
}


float engine::Bone::getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
	float scaleFactor = 0.0f;
	float midWayLength = animationTime - lastTimeStamp;
	float framesDiff = nextTimeStamp - lastTimeStamp;
	scaleFactor = midWayLength / framesDiff;
	return scaleFactor;
}

glm::mat4 engine::Bone::interpolatePosition(float animationTime)
{
	if (1 == m_numPositions)
		return glm::translate(glm::mat4(1.0f), m_positions[0].position);

	int p0Index = getPositionIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = getScaleFactor(m_positions[p0Index].timeStamp, m_positions[p1Index].timeStamp, animationTime);
	glm::vec3 finalPosition = glm::mix(m_positions[p0Index].position, m_positions[p1Index].position, scaleFactor);
	return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 engine::Bone::interpolateRotation(float animationTime)
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

glm::mat4 engine::Bone::interpolateScaling(float animationTime)
{
	if (1 == m_numScalings)
		return glm::scale(glm::mat4(1.0f), m_scales[0].scale);

	int p0Index = getScaleIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = getScaleFactor(m_scales[p0Index].timeStamp, m_scales[p1Index].timeStamp, animationTime);
	glm::vec3 finalScale = glm::mix(m_scales[p0Index].scale, m_scales[p1Index].scale, scaleFactor);
	return glm::scale(glm::mat4(1.0f), finalScale);
}

engine::Bone::~Bone()
{
	logger.trace("Bone destructor called");
}