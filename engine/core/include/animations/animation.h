#pragma once

#include <vector>
#include <map>
#include <functional>

#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

#include "../anim_transform.h"

#include "../models/model.h"
#include "../models/bone.h"
#include "../models/assimp_glm_helpers.h"


namespace engine
{
	enum class AnimationType { undefined = 0, bones = 1, transform = 2 };

	class Animation : private NonCopyable
	{
	public:
		Animation() = default;
		Animation(const std::string& animationName, std::shared_ptr<Model> model, float speedFactor = 1.0f);

		virtual ~Animation();

		virtual AnimationType getTypeID() const
		{
			return AnimationType::undefined;
		}

		inline std::string& getName() { return m_name; }
		void setName(const std::string& name) { m_name = name; }

		inline std::shared_ptr<Model> getModel() { return m_model; }
		void setModel(std::shared_ptr<Model> model) { m_model = model; }

		inline float& getSpeedFactor() { return m_speedFactor; }
		void setSpeedFactor(float speedFactor) { m_speedFactor = speedFactor; }

		inline unsigned int getTicksPerSecond() const { return m_ticksPerSecond; }
		inline float getDuration() const { return m_duration; }
		inline float getDurationInSeconds() const { return m_durationInSeconds; }
		inline unsigned int getFramesCount() const { return m_numFrames; }
		inline unsigned int getFPS() const { return m_desiredFPS; }
		


	protected:
		std::string m_name{};
		std::shared_ptr<Model> m_model{};
		float m_speedFactor{};

		float m_duration{};
		float m_durationInSeconds{};
		unsigned int m_ticksPerSecond{};
		unsigned int m_desiredFPS{ 30u };
		unsigned int m_numFrames{};
	};
}