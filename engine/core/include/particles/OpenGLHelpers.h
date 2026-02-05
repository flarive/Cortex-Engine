#pragma once

#include "../common_defines.h"
//
//#include "stb_image.hpp"
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//
////GLM
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include <glm/ext.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "Global.h"
#include "ParticleSystem.h"
#include "OpenGLData.h"

#include "../shader.h"


namespace engine
{
	class OpenGLHelpers {
	public:
		void selectShaders(char*& vertexShaderSource, char*& fragmentShaderSource, char*& geometryShaderSource);


		void setUpVertexData(OpenGLData& glData);
		int loadTexture();

		void basicDataPrep(OpenGLData& glData, engine::ParticleSystem& ps);
		void geometryDataPrep(OpenGLData& glData, engine::ParticleSystem& ps);
		void instancedDataPrep(OpenGLData& glData, engine::ParticleSystem& ps);

		void basicRender(OpenGLData& glData, int numOfSquares);
		void geometryRender(OpenGLData& glData, int currentDataSize);
		void instancedRender(OpenGLData& glData, int currentDataSize);

		void renderPrep(int mode, OpenGLData& glData, unsigned int VAO);

	private:
		Shader m_shaderSourceBasic{};
		Shader m_shaderSourceGeometry{};
		Shader m_shaderSourceInstanced{};
	};
}
