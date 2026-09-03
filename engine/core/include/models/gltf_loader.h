#pragma once

#include "mesh_loader.h"

// https://github.com/syoyo/tinygltf
#include "tiny_gltf_v3.h"

namespace engine
{
	struct GLTFNode final
	{
		int index;
		int parent;
		std::vector<int> children;

		glm::mat4 local;
		glm::mat4 global;
	};

	struct GLTFSkin
	{
		std::vector<int> joints;        // glTF node indices
		std::vector<glm::mat4> inverseBindMatrices;
	};

	
	class GLtfMeshLoader final : public MeshLoader
	{
	public:
		GLtfMeshLoader() = default;
		virtual ~GLtfMeshLoader();
		void loadModel(const std::string& path, bool loadAnimation = true, bool flipUVs = false) override;

	private:
		std::vector<GLTFNode> m_nodes{};



		std::vector<int> m_jointToBone{};     // size = skin.joints.size()


		
		std::string toStdString(tg3_str s);
		unsigned char* toUChar(tg3_span_u8 span);

		void processNode(const tg3_model& model);
		std::shared_ptr<Mesh> processMesh(const tg3_mesh& mesh, const tg3_model& model, int nodeIndex);
		
		std::shared_ptr<engine::Material> loadPBRMaterial(uint32_t matIndex, const tg3_model& raw);
		std::shared_ptr<engine::Material> loadBlinnPhongMaterial(uint32_t matIndex, const tg3_model& raw);

		std::string getTexture(const tg3_model& raw, const tg3_texture_info& info);
		std::string getTexture(const tg3_model& raw, const tg3_normal_texture_info& info);
		std::string getTexture(const tg3_model& raw, const tg3_occlusion_texture_info& info);

		void extractSkinBones(const tg3_model& raw);
		glm::mat4 getNodeLocalTransform(const tg3_node& n);

		int getTextureSource(const tg3_texture& tex);
		int toInt(const tg3_value& v);

		void computeBindPoseMatrices();
	};
}

