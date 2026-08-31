#pragma once

#include "mesh_loader.h"

#include <assimp/importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace engine
{
	class AssimpMeshLoader final : public MeshLoader
	{
	public:
		AssimpMeshLoader() = default;
		virtual ~AssimpMeshLoader();
		void loadModel(const std::string& path, bool flipUVs) override;

	private:
		
		std::vector<SkeletonBone> m_skeleton{};
		std::vector<glm::mat4> m_finalBindPoseMatrices{};

		
		// processes a node in a recursive fashion.
		// Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
		void processNode(aiNode* node, const aiScene* scene);

		std::shared_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);

		
		void extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

		bool isARMSingleTexture(const aiScene* scene, aiMaterial* mat);
		bool isMRSingleTexture(const aiScene* scene, aiMaterial* mat);

		std::shared_ptr<Material> loadPBRMaterial(const aiScene* scene, aiMaterial* mat);
		std::shared_ptr<Material> loadBlinnPhongMaterial(const aiScene* scene, aiMaterial* mat);
		std::string getTexture(const aiScene* scene, aiMaterial* mat, aiTextureType type);

		void buildSkeletonFromAssimpScene(const aiScene* scene);


		glm::mat4 computeGlobalFromSkeleton(int index);
		void computeBindPoseMatrices();
	};
}
