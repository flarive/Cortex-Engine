#pragma once

#include "animation_loader.h"

#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

#include "../../models/loaders/assimp_glm_helpers.h"

namespace engine
{
	class AssimpAnimationLoader final : public AnimationLoader
	{
	public:
		AssimpAnimationLoader();
		~AssimpAnimationLoader() override;

		void loadAnimation(const std::string& animationPath, std::shared_ptr<Model> model) override;

	private:
		void importBoneAnimation(const std::string& animationPath, std::shared_ptr<Model> model);
		void readMissingBones(const aiAnimation* animation, Model& model);
		void readHierarchyData(AnimNodeData& dest, const aiNode* src);
		unsigned int computeFPS(const aiAnimation* anim);
		void extractBoneKeys(const aiNodeAnim* channel, std::vector<KeyPosition>& positions, std::vector<KeyRotation>& rotations, std::vector<KeyScale>& scales);
	};
}