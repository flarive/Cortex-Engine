#pragma once

#include"particle.h"

#include "../common_defines.h"

#include "../shader.h"
#include "../materials/material.h"
#include "../uvmapping.h"
#include "../transform.h"
#include "../misc/ordered_map.h"

#include<iostream>
#include <stdlib.h>   
#include <time.h>       
#include <vector>

namespace engine
{
	enum class ParticleSystemType { undefined = 0, basic = 1 };

	const std::unordered_map<ParticleSystemType, std::string> ParticleSystemTypeNames = {
		{ParticleSystemType::undefined, "undefined"},
		{ParticleSystemType::basic, "Basic"}
	};

	inline std::string to_string(ParticleSystemType type) {
		auto it = ParticleSystemTypeNames.find(type);
		return it != ParticleSystemTypeNames.end() ? it->second : "unknown";
	}


	class ParticleSystem
	{
	public:
		ParticleSystem();
		ParticleSystem(int _NMAX, int _numOfParticlesPerSecond);
		~ParticleSystem();
		void initParticles(int n);
		void destroyParticle(int index);
		int getCurrentDataSize();
		std::vector<glm::vec3> getDataSquarePoints();
		void getSquareFromCenter(glm::vec3 center);
		void init();
		void setup(const std::shared_ptr<Material>& material, const UvMapping& uv);
		void update();
		void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform);
		std::vector<glm::vec3> getDataCenterPoints();

		ordered_map<std::string, EditorProperty> getPublicProperties() {
			return {
			};
		}

		std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() {
			return {
			};
		}

		ParticleSystemType getTypeID() const
		{
			return ParticleSystemType::basic;
		}



		bool isEnabled() const { return m_isEnabled; }
		void setEnabled(bool enabled) { m_isEnabled = enabled; }

		glm::vec3& getPosition() { return m_position; }
		glm::vec3& getRotation() { return m_rotation; }
		glm::vec3& getScale() { return m_scale; }

		void setPosition(const glm::vec3& position) { m_position = position; }
		void setRotation(const glm::vec3& rotation) { m_rotation = rotation; }
		void setScale(const glm::vec3& scale) { m_scale = scale; }

		void setTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
			m_position = position;
			m_rotation = rotation;
			m_scale = scale;
		}

		float getSquareSize() { return m_squareSize; }

		Particle* getParticleArray() { return m_particlesArray; }

		int getMaxFilledIndex() { return m_maxFilledIndex; }

		bool* getFlags() { return m_flags; }
	

		void reSetup() {}

		//void basicDataPrep();
		//void geometryDataPrep();
		//void instancedDataPrep();


		//void renderPrep(unsigned int VAO);


		//void basicRender(int numOfSquares);
		//void geometryRender(int currentDataSize);
		//void instancedRender(int currentDataSize);

	private:
		int m_NMAX{};
		int m_numOfParticlesPerSecond{};
		int m_nbFrames{};
		int m_currentDataSize{};
		int m_partCounter{};
		int m_maxFilledIndex{};

		double m_prevTime{};
		double m_fpsTime{};

		float m_squareSize{};
		bool* m_flags{};

		bool m_isEnabled{ true };

		Particle* m_particlesArray{};
		glm::vec3 m_squarePoints[4];

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

		std::shared_ptr<Material> m_material{};

		Color m_ambientColor{};

		float m_uvScale{ 1.0f };

		glm::vec3 m_position{};
		glm::vec3 m_rotation{};
		glm::vec3 m_scale{};

		unsigned int m_VAO{};
		unsigned int m_VBO{};
		unsigned int m_EBO{};

		void geometrySetup();

		void basicDataPrep();
	};
}