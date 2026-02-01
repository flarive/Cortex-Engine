#pragma once

#include "../common_defines.h"
#include "../cameras/camera.h"
#include "../shader.h"

// https://github.com/TheCherno/OneHourParticleSystem/blob/master/OpenGL-Sandbox/src/ParticleSystem.h
namespace engine
{
	struct ParticleProps
	{
		glm::vec2 Position;
		glm::vec2 Velocity, VelocityVariation;
		glm::vec4 ColorBegin, ColorEnd;
		float SizeBegin, SizeEnd, SizeVariation;
		float LifeTime = 1.0f;
	};

	class ParticleSystem
	{
	public:
		ParticleSystem();

		void OnUpdate(float ts);
		void OnRender(std::shared_ptr<Camera> camera);

		void Emit(const ParticleProps& particleProps);
	private:
		struct Particle
		{
			glm::vec2 Position{};
			glm::vec2 Velocity{};
			glm::vec4 ColorBegin{}, ColorEnd{};
			float Rotation{};
			float SizeBegin{}, SizeEnd{};

			float LifeTime = 1.0f;
			float LifeRemaining = 0.0f;

			bool Active = false;
		};
		
		Shader m_shader{};
		
		std::vector<Particle> m_ParticlePool{};
		uint32_t m_PoolIndex = 999;

		GLuint m_QuadVA = 0;
		GLint m_ParticleShaderViewProj{}, m_ParticleShaderTransform{}, m_ParticleShaderColor{};
	};
}