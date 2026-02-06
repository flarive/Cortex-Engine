#include "../../include/particles/particlesystem.h"

#include "../../include/texture.h"

#include "../../include/debug/opengl_debug.h"

using namespace std;

engine::ParticleSystem::ParticleSystem()
	: m_maxParticles(100), m_numOfParticlesPerSecond(10), m_squareSize(0.25f)
{
	init();
}

engine::ParticleSystem::ParticleSystem(int _maxParticles, int _numOfParticlesPerSecond, float _particleSize)
	: m_maxParticles(_maxParticles), m_numOfParticlesPerSecond(_numOfParticlesPerSecond), m_squareSize(_particleSize)
{
	init();
}

engine::ParticleSystem::~ParticleSystem()
{
	delete(m_particlesArray);
	delete(m_flags);
}

void engine::ParticleSystem::setup(const std::shared_ptr<Material>& material, const UvMapping& uv)
{
	m_material = material;
	m_uvScale = uv.getUvScale();

	geometrySetup();

	if (material && material->hasDiffuseMap())
		material->loadTexturesAsync();
}

void engine::ParticleSystem::geometrySetup()
{
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);


	//if (Global::drawtype == Instanced) {
	//	glGenVertexArrays(1, &glData.quadVAO);
	//	glGenBuffers(1, &glData.quadVBO);
	//	glBindVertexArray(glData.quadVAO);

	//	float particleSize = Global::particleSize;
	//	glGenBuffers(1, &glData.instanceVBO);

	//	float quadVertices[] = {
	//		// positions    
	//		-particleSize,  particleSize,   0.0f, 1.0f,    // top left
	//		 particleSize, -particleSize,   1.0f, 0.0f,   // bottom right
	//		-particleSize, -particleSize,   0.0f, 0.0f,   // bottom left

	//		-particleSize,  particleSize,   0.0f, 1.0f,    // top left
	//		 particleSize,  -particleSize,  1.0f, 0.0f,   // bottom right
	//		 particleSize,  particleSize,   1.0f, 1.0f,   // top right 
	//	};
	//	glBindBuffer(GL_ARRAY_BUFFER, glData.quadVBO);
	//	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	//	glEnableVertexAttribArray(0);
	//	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	//	glEnableVertexAttribArray(1);
	//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	//}
}

void engine::ParticleSystem::init()
{
	m_prevTime = glfwGetTime();
	m_fpsTime = glfwGetTime();

	m_partCounter = 0;
	m_nbFrames = 0;
	m_maxFilledIndex = 0;
	m_currentDataSize = 0;
	m_squareSize = 0.25;

	m_particlesArray = new Particle[m_maxParticles];
	m_flags = new bool[m_maxParticles];

	for (size_t i = 0; i < m_maxParticles; i++)
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

void engine::ParticleSystem::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform)
{
	if (!m_isEnabled)
		return;

	ShaderType type = shader.getShaderType();

	if (!m_material || !shader.isValid()) {
		std::cerr << "Material or shader not valid. Skipping draw." << std::endl;
		return;
	}

	if (!m_material->areAllTexturesLoaded()) {
		std::cout << "Textures not ready. Deferring draw." << std::endl;
		return;
	}

	if (m_VAO == 0 || m_VBO == 0) {
		std::cerr << "VAO/VBO not initialized. Skipping draw." << std::endl;
		return;
	}

	basicDataPrep();

	shader.use();
	OpenGLDebug::checkGLError("shader.use9999922");

	setTransform(localTransform.getLocalPosition(), localTransform.getLocalRotation(), localTransform.getLocalScale());

	if (m_material)
	{
		if (type == ShaderType::BlinnPhong || type == ShaderType::PBR)
		{
			if (!m_material->bind(shader)) {
				std::cerr << "Failed to bind textures. Skipping draw." << std::endl;
				return;
			}

			shader.setVec3("material.ambient_color", m_material->getAmbientColor());
			shader.setVec3("material.diffuse_color", m_material->getDiffuseColor());
			shader.setVec3("material.specular_color", m_material->getSpecularColor());
			shader.setFloat("material.ambient_intensity", m_material->getAmbientIntensity());
			shader.setFloat("material.normalMapIntensity", m_material->getNormalIntensity());
			shader.setFloat("material.emissiveIntensity", m_material->getEmissiveIntensity());

			shader.setBool("material.canCastShadows", false);
			shader.setBool("material.canReceiveShadows", false);
		}

		// used by all shaders (blinnphong, pbr, simpleDepthBuffer1, simpleDepthBuffer2)
		shader.setMat4("model", transformMatrix);

		if (type == ShaderType::BlinnPhong || type == ShaderType::PBR)
		{
			shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(transformMatrix))));
			shader.setBool("hasTangents", true);
		}
	}

	// Send to GPU
	glBindVertexArray(m_VAO);
	OpenGLDebug::checkGLError("glBindVertexArray");

	
	int numOfSquares = getCurrentDataSize() / 8;
	glDrawElements(GL_TRIANGLES, numOfSquares * 6, GL_UNSIGNED_INT, 0);
	OpenGLDebug::checkGLError("glDrawArrays");


	glBindVertexArray(0);
	OpenGLDebug::checkGLError("glBindVertexArray");

	if (m_material && (type == ShaderType::BlinnPhong || type == ShaderType::PBR))
	{
		m_material->unbind(); // Unbind textures to prevent OpenGL state retention
		OpenGLDebug::checkGLError("Unbind");
	}
}

void engine::ParticleSystem::basicDataPrep()
{
	std::vector<glm::vec3>  points = getDataSquarePoints();
	int currentDataSize = getCurrentDataSize();

	glm::vec3* data = points.data();

	int numOfSquares = (currentDataSize / 8);
	std::vector<int> indices;
	for (size_t i = 0; i < numOfSquares; i++)
	{
		indices.push_back(i * 4);
		indices.push_back(i * 4 + 1);
		indices.push_back(i * 4 + 3);
		indices.push_back(i * 4 + 1);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 3);
	}
	int* indicesData = indices.data();

	if (currentDataSize > 0) {
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * currentDataSize, data, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numOfSquares * 6 * sizeof(int), indicesData, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)(sizeof(glm::vec3)));
		glEnableVertexAttribArray(1);
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
		for (size_t j = 0; j < m_maxParticles; j++)
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
