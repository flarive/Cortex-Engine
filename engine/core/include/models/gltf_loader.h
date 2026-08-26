#pragma once

#include "mesh_loader.h"

// https://github.com/syoyo/tinygltf
#include "tiny_gltf_v3.h"

namespace engine
{
	class GLtfMeshLoader final : public MeshLoader
	{
	public:
		GLtfMeshLoader() = default;
		virtual ~GLtfMeshLoader();
		void loadModel(const std::string& path, bool flipUVs) override;

	private:
		std::string toStdString(tg3_str s);
		unsigned char* toUChar(tg3_span_u8 span);


		std::shared_ptr<Mesh> processMesh(const tg3_mesh& mesh, const tg3_model& raw);
		
		std::shared_ptr<engine::Material> loadPBRMaterial(uint32_t matIndex, const tg3_model& raw);
		std::shared_ptr<engine::Material> loadBlinnPhongMaterial(uint32_t matIndex, const tg3_model& raw);

		std::string getTexture(const tg3_model& raw, const tg3_texture_info& info);
		std::string getTexture(const tg3_model& raw, const tg3_normal_texture_info& info);
		std::string getTexture(const tg3_model& raw, const tg3_occlusion_texture_info& info);

		int getTextureSource(const tg3_texture& tex);
		int toInt(const tg3_value& v);
	};
}

