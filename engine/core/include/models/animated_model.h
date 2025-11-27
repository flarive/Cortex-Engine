#pragma once

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../misc/noncopyable.h"
#include "../texture.h"
#include "model.h"
#include "mesh.h"
#include "../shader.h"
#include "../transform.h"


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include "assimp_glm_helpers.h"
#include "animdata.h"

namespace engine
{
	class AnimatedModel final : public Model
	{
	public:

		auto& GetBoneInfoMap() { return m_BoneInfoMap; }
		int& GetBoneCount() { return m_BoneCounter; }
		

	private:

		std::map<std::string, BoneInfo> m_BoneInfoMap{};
		int m_BoneCounter{0};


		void SetVertexBoneDataToDefault(Vertex& vertex);
	
	};
}