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
	};
}

