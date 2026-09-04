#pragma once

#include <vector>
#include <map>
#include <functional>

#include "animation.h"
#include "../models/model.h"
#include "../models/bone.h"

#include "../animations/loaders/animation_loader.h"

namespace engine
{
	class BoneAnimation final : public Animation
	{
	public:
		BoneAnimation() = default;
		BoneAnimation(const std::string& animationName, const std::string& animationPath, std::shared_ptr<Model> model, float speedFactor = 1.0f);

		~BoneAnimation() override;

		AnimationType getTypeID() const override
		{
			return AnimationType::bones;
		}




		inline std::string& getFilepath() { return m_filepath; }
		void setFilepath(const std::string& filepath) { m_filepath = filepath; }

		Bone* findBone(const std::string& name);
		
		


		const std::vector<Bone>& getBones() const;
		const size_t getBoneCount() const;
		const AnimNodeData& getRootNode() const;
		const std::map<std::string, BoneInfo>& getBonesInfoMap() const;



	private:
		std::string m_filepath{};
		std::unique_ptr<AnimationLoader> m_animationLoader{};
	};
}