#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"

#include "../misc/ordered_map.h"
#include "animation.h"

#include <vector>
#include <map>
#include <unordered_map>
#include <functional>

namespace engine
{
	enum class AnimatorType { undefined = 0, bones = 1 };

	const std::unordered_map<AnimatorType, std::string> AnimatorTypeNames = {
	{AnimatorType::undefined, "undefined"},
	{AnimatorType::bones, "bones"}
	};

	inline std::string to_string(AnimatorType type) {
		auto it = AnimatorTypeNames.find(type);
		return it != AnimatorTypeNames.end() ? it->second : "unknown";
	}



    
    class Animator : private NonCopyable
    {
    public:
		Animator(std::shared_ptr<Animation> animation);
		Animator(std::vector<std::shared_ptr<Animation>>& animations);
		~Animator() = default;

		virtual AnimatorType getTypeID() const
		{
			return AnimatorType::undefined;
		}

		virtual ordered_map<std::string, EditorProperty> getPublicProperties() = 0;
		virtual std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() = 0;

		virtual void updateAnimation(float dt) = 0;
		virtual void playAnimation(std::shared_ptr<Animation> pAnimation) = 0;

		virtual std::vector<glm::mat4> getFinalBoneMatrices() = 0;

	protected:
		std::vector<std::shared_ptr<Animation>> m_animations{};
		std::shared_ptr<Animation> m_CurrentAnimation{};
		float m_CurrentTime{};
		float m_DeltaTime{};
    };
}