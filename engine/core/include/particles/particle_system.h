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
#include <string>

namespace engine
{
	enum class ParticleSystemType { undefined = 0, basic = 1, geometry = 2, instanced = 3 };

	const std::unordered_map<ParticleSystemType, std::string> ParticleSystemTypeNames = {
		{ParticleSystemType::undefined, "undefined"},
		{ParticleSystemType::basic, "Basic"}
	};

	inline std::string to_string(ParticleSystemType type) {
		auto it = ParticleSystemTypeNames.find(type);
		return it != ParticleSystemTypeNames.end() ? it->second : "unknown";
	}


	class ParticleSystem final
	{
	public:
		ParticleSystem();
		ParticleSystem(unsigned int _maxParticles, unsigned int _numOfParticlesPerSecond, float _particleSize, float _emitterRadius = 1.0f, float _lifeSpan = -0.25f, bool _infiniteEmission = true);
		~ParticleSystem();
		void initParticles(unsigned int n);
		void destroyParticle(unsigned int index);
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
				{"MaxParticlesCount", EditorProperty { "Max particles count", getMaxParticles(), editable, 0.0f, 10000.0f, 10.0f, ""}},
				{"NbrParticlesPerSecond", EditorProperty { "Nbr particles/sec", getNumOfParticlesPerSecond(), editable, 0.0f, 10000.0f, 10.0f, ""}},
				{"ParticleSize", EditorProperty { "Particles size", getParticleSize(), editable, 0.0f, 10.0f, 0.1f, "%.2f"}},
				{"EmitterRadius", EditorProperty { "Emitter radius", getEmitterRadius(), editable, 0.0f, 10.0f, 0.1f, "%.2f"}},
				{"LifeSpan", EditorProperty { "Life span", getLifeSpan(), editable, -1.0f, 1.0f, 0.01f, "%.2f"}},
				{"InfiniteEmission", EditorProperty { "Infinite emission", getInfiniteEmission(), editable, 0.0f, 0.0f, 0.0f, ""}},
				{"RenderMode", EditorProperty { "Mode", getModesList(), editable | combobox, 0.0f, 0.0f, 0.0f, "", "", [this](unsigned short index) { setModeAtIndex(index); }}}
			};
		}

		std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() {
			return {
				{ "MaxParticlesCount", [this](EditorPropertyValue value) { getMaxParticles() = *(std::get_if<unsigned int>(&value)); } },
				{ "NbrParticlesPerSecond", [this](EditorPropertyValue value) { getNumOfParticlesPerSecond() = *(std::get_if<unsigned int>(&value)); } },
				{ "ParticleSize", [this](EditorPropertyValue value) { getParticleSize() = *(std::get_if<float>(&value)); } },
				{ "EmitterRadius", [this](EditorPropertyValue value) { getEmitterRadius() = *(std::get_if<float>(&value)); } },
				{ "LifeSpan", [this](EditorPropertyValue value) { getLifeSpan() = *(std::get_if<float>(&value)); } },
				{ "InfiniteEmission", [this](EditorPropertyValue value) { getInfiniteEmission() = *(std::get_if<bool>(&value)); } }
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
	

		void reSetup() { init(); }

		unsigned int& getMaxParticles() { return m_maxParticles; }
		void setMaxParticles(unsigned int _max) { m_maxParticles = _max; }

		unsigned int& getNumOfParticlesPerSecond() { return m_numOfParticlesPerSecond; }
		void setNumOfParticlesPerSecond(unsigned int _num) { m_numOfParticlesPerSecond = _num; }

		float& getParticleSize() { return m_squareSize; }
		void setParticleSize(float _size) { m_squareSize = _size; }

		float& getEmitterRadius() { return m_emitterRadius; }
		void setEmitterRadius(float _radius) { m_emitterRadius = _radius; }

		float& getLifeSpan() { return m_lifeSpan; }
		void setLifeSpan(float _lifeSpan) { m_lifeSpan = _lifeSpan; }

		bool& getInfiniteEmission() { return m_infiniteEmission; }
		void setInfiniteEmission(bool _infiniteEmission) { m_infiniteEmission = _infiniteEmission; }

		void resetDrawCallCount() { m_drawCallCount = 0; }
		unsigned int getDrawCallCount() const { return m_drawCallCount; }

		std::vector<std::string> getModesList();
		void setModeAtIndex(unsigned short index);

	private:
		unsigned int m_maxParticles{};
		unsigned int m_numOfParticlesPerSecond{};
		unsigned int m_nbFrames{};
		unsigned int m_currentDataSize{};
		unsigned int m_partCounter{};
		unsigned int m_maxFilledIndex{};
		float m_emitterRadius{};
		float m_lifeSpan{};
		bool m_infiniteEmission{ true };

		double m_prevTime{};
		double m_fpsTime{};

		float m_squareSize{};
		bool* m_flags{};

		bool m_isEnabled{ true };

		Particle* m_particlesArray{};
		glm::vec3 m_squarePoints[4];

		ParticleSystemType m_type = ParticleSystemType::basic;


		std::shared_ptr<Material> m_material{};
		Shader m_shaderSourceBasic{};
		Shader m_shaderSourceGeometry{};
		Shader m_shaderSourceInstanced{};

		float m_uvScale{ 1.0f };

		glm::vec3 m_position{};
		glm::vec3 m_rotation{};
		glm::vec3 m_scale{};

		// basic
		unsigned int m_VAO{};
		unsigned int m_VBO{};
		unsigned int m_EBO{};

		// geometry
		unsigned int m_quadVAO{};
		unsigned int m_quadVBO{};
		
		// instanced
		unsigned int m_instanceVBO{};

		unsigned int m_drawCallCount{};

		void geometrySetup();

		void basicDataPrep();
		void geometryDataPrep();
		void instancedDataPrep();
	};
}