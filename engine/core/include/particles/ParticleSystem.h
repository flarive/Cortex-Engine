#pragma once

#include"particle.h"

#include "../common_defines.h"

#include "../shader.h"
#include "../misc/ordered_map.h"

#include<iostream>
#include <stdlib.h>   
#include <time.h>       
#include <vector>

namespace engine
{
	//enum class ParticleDrawType { basic = 0, geometry = 1, instanced = 2 };
	
	class ParticleSystem
	{
	public:
		ParticleSystem(int _NMAX, int _numOfParticlesPerSecond);
		~ParticleSystem();
		void initParticles(int n);
		void destroyParticle(int index);
		int getCurrentDataSize();
		std::vector<glm::vec3> getDataSquarePoints();
		void getSquareFromCenter(glm::vec3 center);
		void update();
		std::vector<glm::vec3> getDataCenterPoints();

		ordered_map<std::string, EditorProperty> getPublicProperties() {
			return {
				//{"uvscale", EditorProperty { "UV scale", getUvScale(), editable, 0.0f, 10.0f, 0.01f, "%.3f"}},
				//{"canCastShadows", EditorProperty { "Cast shadows", canCastShadows(), editable, 0.0f, 10.0f, 0.01f, "%.3f" }},
				//{"canReceiveShadows", EditorProperty { "Receive shadows", canReceiveShadows(), editable, 0.0f, 10.0f, 0.01f, "%.3f" }}
			};
		}

		std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() {
			return {
				//{ "uvscale", [this](EditorPropertyValue value) { getUvScale() = *(std::get_if<float>(&value)); } },
				//{ "canCastShadows", [this](EditorPropertyValue value) { canCastShadows() = *(std::get_if<bool>(&value)); } },
				//{ "canReceiveShadows", [this](EditorPropertyValue value) { canReceiveShadows() = *(std::get_if<bool>(&value)); } }
			};
		}

		bool isEnabled() const { return m_isEnabled; }
		void setEnabled(bool enabled) { m_isEnabled = enabled; }

		void reSetup() {}

		//void basicDataPrep();
		//void geometryDataPrep();
		//void instancedDataPrep();


		//void renderPrep(unsigned int VAO);


		//void basicRender(int numOfSquares);
		//void geometryRender(int currentDataSize);
		//void instancedRender(int currentDataSize);

	private:
		int NMAX{};
		int numOfParticlesPerSecond{};
		int nbFrames{};
		int currentDataSize{};
		int partCounter{};
		int maxFilledIndex{};

		double prevTime{};
		double fpsTime{};

		float squareSize{};
		bool* flags{};

		Particle* particlesArray{};
		glm::vec3 squarePoints[4];

		//Shader m_shaderSourceBasic{};
		//Shader m_shaderSourceGeometry{};
		//Shader m_shaderSourceInstanced{};



		//unsigned int id_shader_PVM_uniform{};
		//unsigned int id_shader_SquareSize_uniform{};
		//unsigned int VAO{};
		//unsigned int VBO{};
		//unsigned int EBO{};
		//unsigned int quadVAO{};
		//unsigned int quadVBO{};
		//unsigned int instanceVBO{};

		////int shaderProgram{};
		//int texture{};

		//glm::mat4 PVM{};

		//void setUpVertexData();

		bool m_isEnabled{true};

		glm::vec3 m_position{};
		glm::vec3 m_rotation{};
		glm::vec3 m_scale{};
	};
}