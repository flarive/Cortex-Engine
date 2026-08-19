#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"

#include "mesh.h"
#include "bone.h"

namespace engine
{
	/// <summary>
	/// Abstract class for mesh loaders
	/// </summary>
	class MeshLoader : public NonCopyable
	{
	public:
		MeshLoader() = default;
		virtual ~MeshLoader();

		virtual void loadModel(const std::string& path, bool flipUVs) = 0;
		
		static std::unique_ptr<MeshLoader> create(const std::string& path);

		const std::vector<std::string> getRequestLoadingTextures() const { return m_requestLoadingTextures; }

		unsigned int getMeshCount() const { return m_numberOfMeshes; }
		unsigned int getVertexCount() const { return m_numberOfVertices; }

		std::vector<std::shared_ptr<Mesh>>& getMeshes() { return m_meshes; }

	protected:
		std::string m_directory{};
		std::string m_filename{};
		unsigned int m_numberOfMeshes{};
		unsigned int m_numberOfVertices{};

		std::vector<std::shared_ptr<Mesh>> m_meshes{};

		std::vector<std::shared_ptr<Material>> m_materials{};

		std::vector<std::string> m_requestLoadingTextures{};	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	};
}
