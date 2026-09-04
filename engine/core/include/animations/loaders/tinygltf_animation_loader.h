#pragma once

#include "animation_loader.h"

#include "tiny_gltf_v3.h"

namespace engine
{
	class TinygltfAnimationLoader final : public AnimationLoader
	{
	public:
	public:
		TinygltfAnimationLoader();
		~TinygltfAnimationLoader() override;

		void loadAnimation(const std::string& animationPath, std::shared_ptr<Model> model) override;

	private:
		void importBoneAnimation(const std::string& animationPath, std::shared_ptr<Model> model);
		void readMissingBones(const tg3_model& gltfModel, const tg3_animation& animation, Model& engineModel);
		void readHierarchyData(AnimNodeData& dest, const tg3_model& model, const tg3_node& src);
		unsigned int computeFPS(const tg3_model& model, const tg3_animation& anim);
		float computeDuration(const tg3_model& model, const tg3_animation& anim);
		void extractBoneKeys(const tg3_model& model, const tg3_animation& anim, const tg3_animation_channel& channel, std::vector<KeyPosition>& positions, std::vector<KeyRotation>& rotations, std::vector<KeyScale>& scales);
	};
}