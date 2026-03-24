#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"

#include "../misc/ordered_map.h"
#include "../animations/animation.h"
#include "../animations/bone_animation.h"
#include "../shader.h"

#include <vector>
#include <map>
#include <unordered_map>
#include <functional>

namespace engine
{
	enum class AnimatorType { undefined = 0, bones = 1, transform = 2 };

	const std::unordered_map<AnimatorType, std::string> AnimatorTypeNames = {
		{AnimatorType::undefined, "undefined"},
		{AnimatorType::bones, "bones"},
		{AnimatorType::transform, "transform"},
	};

	inline std::string to_string(AnimatorType type) {
		auto it = AnimatorTypeNames.find(type);
		return it != AnimatorTypeNames.end() ? it->second : "unknown";
	}



    
    class Animator : private NonCopyable
    {
    public:
		Animator(std::shared_ptr<BoneAnimation> animation);
		Animator(const std::vector<std::shared_ptr<BoneAnimation>>& animations);
		~Animator() = default;

		virtual AnimatorType getTypeID() const
		{
			return AnimatorType::undefined;
		}

		virtual ordered_map<std::string, EditorProperty> getPublicProperties() = 0;
		virtual std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() = 0;

		virtual void update(float dt) = 0;
		virtual void draw(Shader& shader, Transform& localTransform) = 0;

		virtual void playAnimation(std::shared_ptr<BoneAnimation> pAnimation) = 0;
		virtual void playAnimation() = 0;
		virtual void stopAnimation() = 0;

		virtual void playAnimationAtIndex(unsigned short index) = 0;

		void setBoneCount(unsigned int boneCount) { m_boneCount = boneCount; }


	protected:
		std::vector<std::shared_ptr<BoneAnimation>> m_animations{};
		std::shared_ptr<BoneAnimation> m_currentAnimation{};
		float m_CurrentTime{};
		float m_DeltaTime{};
		bool m_isPlaying{ false };

		size_t m_boneCount{};
    };
}