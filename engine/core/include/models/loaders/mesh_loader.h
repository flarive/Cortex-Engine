#pragma once

#include "../../misc/noncopyable.h"
#include "../../common_defines.h"

#include "../mesh.h"
#include "../skeleton.h"
#include "../bone.h"

namespace engine
{
	/// <summary>
	/// Abstract class for mesh loaders
	/// </summary>
	class MeshLoader : public NonCopyable
	{
	public:
		MeshLoader();
		virtual ~MeshLoader();

		virtual void loadModel(const std::string& path, bool loadAnimation, bool flipUVs) = 0;
		
		static std::unique_ptr<MeshLoader> create(const std::string& path);

		const std::vector<std::string> getRequestLoadingTextures() const { return m_requestLoadingTextures; }

		unsigned int getMeshCount() const { return m_numberOfMeshes; }
		unsigned int getVertexCount() const { return m_numberOfVertices; }

		std::vector<std::shared_ptr<Mesh>>& getMeshes() { return m_meshes; }
		std::vector<std::shared_ptr<Material>>& getMaterials() { return m_materials; }
		std::shared_ptr<Skeleton>& getSkeleton() { return m_skeleton; }


		std::map<std::string, BoneInfo>& getBoneInfoMap();
		unsigned int getBoneCount();
		bool& hasBones() { return m_hasBones; }
		bool& hasAnimations() { return m_hasAnimations; }

		

		int getSkeletonRootIndex() const;


	protected:
		std::string m_directory{};
		std::string m_filename{};

		unsigned int m_numberOfMeshes{};
		unsigned int m_numberOfVertices{};

		std::vector<std::shared_ptr<Mesh>> m_meshes{};

		std::vector<std::shared_ptr<Material>> m_materials{};

		std::vector<std::string> m_requestLoadingTextures{};	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.

		// bones
		bool m_hasBones{};
		std::shared_ptr<Skeleton> m_skeleton{};

		// animations
		bool m_hasAnimations{ false };
		std::vector<glm::mat4> m_finalBindPoseMatrices{};

		void setVertexBoneDataToDefault(Vertex& vertex);
		void setVertexBoneData(Vertex& vertex, int boneID, float weight);
	};
}
