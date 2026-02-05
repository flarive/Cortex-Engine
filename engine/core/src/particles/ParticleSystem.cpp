#include "../../include/particles/particlesystem.h"

#include "../../include/texture.h"

using namespace std;

engine::ParticleSystem::ParticleSystem()
	: m_NMAX(100), m_numOfParticlesPerSecond(10)
{
	setup();
}

engine::ParticleSystem::ParticleSystem(int _NMAX, int _numOfParticlesPerSecond)
	: m_NMAX(_NMAX), m_numOfParticlesPerSecond(_numOfParticlesPerSecond)
{
	setup();
}

engine::ParticleSystem::~ParticleSystem()
{
	delete(m_particlesArray);
	delete(m_flags);
}


void engine::ParticleSystem::setup()
{
	m_prevTime = glfwGetTime();
	m_fpsTime = glfwGetTime();

	m_partCounter = 0;
	m_nbFrames = 0;
	m_maxFilledIndex = 0;
	m_currentDataSize = 0;
	m_squareSize = 0.25;

	m_particlesArray = new Particle[m_NMAX];
	m_flags = new bool[m_NMAX];

	for (size_t i = 0; i < m_NMAX; i++)
		m_flags[i] = false;
}

void engine::ParticleSystem::update()
{
	double currentTime = glfwGetTime();
	double deltaTime = currentTime - m_prevTime;
	m_prevTime = currentTime;
	m_nbFrames++;

	m_partCounter += ceil(m_numOfParticlesPerSecond * deltaTime);
	initParticles(m_partCounter);

	if (currentTime - m_fpsTime >= 1.0) {
		//cout << "FPS: " << nbFrames << "\n";
		m_fpsTime += 1.0;
		m_nbFrames = 0;
		m_partCounter = 0;
	}

	for (size_t i = 0; i <= m_maxFilledIndex; i++)
	{
		if (m_flags[i] == true) {
			m_particlesArray[i].lifeSpan -= deltaTime;
			m_particlesArray[i].position = m_particlesArray[i].position + m_particlesArray[i].startVel * glm::vec3(deltaTime);
			if (m_particlesArray[i].lifeSpan <= 0) {
				destroyParticle(i);
			}
		}
	}
}

void engine::ParticleSystem::destroyParticle(int index)
{
	m_flags[index] = false;
	if (index == m_maxFilledIndex) {
		for (int i = m_maxFilledIndex; i >= 0; i--)
		{
			if (m_flags[i] == true) {
				m_maxFilledIndex = i;
				break;
			}
		}
	}
}

void engine::ParticleSystem::initParticles(int n)
{
	for (size_t i = 0; i < n; i++)
	{
		Particle particle{};
		for (size_t j = 0; j < m_NMAX; j++)
		{
			if (m_flags[j] == false) {
				m_particlesArray[j] = particle;
				m_flags[j] = true;
				if (j >= m_maxFilledIndex) {
					m_maxFilledIndex = j;
				}
				break;
			}
		}
	}
}

vector<glm::vec3> engine::ParticleSystem::getDataSquarePoints()
{
	int squareDim = 1;
	std::vector<glm::vec3> points;
	m_currentDataSize = 0;
	for (size_t i = 0; i <= m_maxFilledIndex; i++)
	{
		if (m_flags[i] == true) {
			getSquareFromCenter(m_particlesArray[i].position);
			for (size_t i = 0; i < 4; i++) {
				points.push_back(m_squarePoints[i]);
				m_currentDataSize++;
				if (i == 0) {
					points.push_back(glm::vec3(1, 0, 1));
					m_currentDataSize++;
				}
				else if (i == 1) {
					points.push_back(glm::vec3(0, 0, 1));
					m_currentDataSize++;
				}
				else if (i == 2) {
					points.push_back(glm::vec3(0, 1, 1));
					m_currentDataSize++;
				}
				else if (i == 3) {
					points.push_back(glm::vec3(1, 1, 1));
					m_currentDataSize++;
				}
			}
		}
	}
	return points;
}

std::vector<glm::vec3> engine::ParticleSystem::getDataCenterPoints()
{
	int squareDim = 1;
	std::vector<glm::vec3> points;
	m_currentDataSize = 0;
	for (size_t i = 0; i <= m_maxFilledIndex; i++)
	{
		if (m_flags[i] == true) {
			points.push_back(m_particlesArray[i].position);
			m_currentDataSize++;
		}
	}
	return points;
}

void engine::ParticleSystem::getSquareFromCenter(glm::vec3 center)
{
	float dist = m_squareSize;
	//Point down-left
	m_squarePoints[0] = center + glm::vec3(dist, -dist, 0);
	//Point up-left
	m_squarePoints[1] = center + glm::vec3(-dist, -dist, 0);
	//Point down-right
	m_squarePoints[2] = center + glm::vec3(-dist, dist, 0);
	//Point up_right
	m_squarePoints[3] = center + glm::vec3(dist, dist, 0);
}

int engine::ParticleSystem::getCurrentDataSize()
{
	return m_currentDataSize;
}
