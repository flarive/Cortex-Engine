#include "../../include/particles/particle_system.h"

#include "../../include/texture.h"

#include "../../include/debug/opengl_debug.h"

#define M_PI 3.14159265358979323846


using namespace std;

engine::ParticleSystem::ParticleSystem()
	: m_maxParticles(100), m_numOfParticlesPerSecond(10), m_squareSize(0.25f), m_emitterRadius(1.0f), m_lifeSpan(0.25f), m_infiniteEmission(true), m_drawCallCount(0)
{
	init();
}

engine::ParticleSystem::ParticleSystem(unsigned int _maxParticles, unsigned int _numOfParticlesPerSecond, float _particleSize, float _emitterRadius, float _lifeSpan, bool _infiniteEmission)
	: m_maxParticles(_maxParticles), m_numOfParticlesPerSecond(_numOfParticlesPerSecond), m_squareSize(_particleSize), m_emitterRadius(_emitterRadius), m_lifeSpan(_lifeSpan), m_infiniteEmission(_infiniteEmission), m_drawCallCount(0)
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

	m_shaderSourceBasic.init("shaderSourceBasic", "shaders/ParticleSourceBasic.vert", "shaders/ParticleSourceBasic.frag");
	m_shaderSourceGeometry.init("shaderSourceGeometry", "shaders/ParticleSourceGeometry.vert", "shaders/ParticleSourceGeometry.geom", "shaders/ParticleSourceGeometry.frag");
	m_shaderSourceInstanced.init("shaderSourceInstanced", "shaders/ParticleSourceInstanced.vert", "shaders/ParticleSourceBasic.frag");

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

	if (m_type == ParticleSystemType::instanced)
	{
		glGenVertexArrays(1, &m_quadVAO);
		glGenBuffers(1, &m_quadVBO);
		glGenBuffers(1, &m_instanceVBO); // Generate the instanced VBO here

		glBindVertexArray(m_quadVAO);

		float s = m_squareSize;
		float quadVertices[] = {
			// positions (x,y)      // UVs     // CCW
			-s, -s,   0.0f, 0.0f,   // BL
			 s, -s,   1.0f, 0.0f,   // BR
			 s,  s,   1.0f, 1.0f,   // TR

			-s, -s,   0.0f, 0.0f,   // BL
			 s,  s,   1.0f, 1.0f,   // TR
			-s,  s,   0.0f, 1.0f,   // TL
		};


		glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// Unbind the VBO and VAO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void engine::ParticleSystem::init()
{
	geometrySetup();
	
	m_prevTime = glfwGetTime();
	m_fpsTime = glfwGetTime();

	m_partCounter = 0;
	m_nbFrames = 0;
	m_maxFilledIndex = 0;
	m_currentDataSize = 0;

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
		m_fpsTime += 1.0;
		m_nbFrames = 0;
		m_partCounter = 0;
	}

	for (unsigned int i = 0; i <= m_maxFilledIndex; i++)
	{
		if (m_flags[i] == true) {
			
			if (m_infiniteEmission)
				m_particlesArray[i].lifeSpan -= deltaTime;
			else
				m_particlesArray[i].lifeSpan -= m_lifeSpan;

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
	if (type != ShaderType::BlinnPhong && type != ShaderType::PBR) {
		return;
	}

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


	if (m_type == ParticleSystemType::basic)
		basicDataPrep();
	else if (m_type == ParticleSystemType::geometry)
		geometryDataPrep();
	else if (m_type == ParticleSystemType::instanced)
		instancedDataPrep();

	setTransform(localTransform.getLocalPosition(), localTransform.getLocalRotation(), localTransform.getLocalScale());


	if (m_type == ParticleSystemType::basic)
	{
		if (!m_material->bind2(m_shaderSourceBasic)) {
			std::cerr << "Failed to bind textures. Skipping draw." << std::endl;
			return;
		}

		m_shaderSourceBasic.setMat4("model", transformMatrix);
		m_shaderSourceBasic.setMat4("view", view);
		m_shaderSourceBasic.setMat4("projection", projection);
	}
	else if (m_type == ParticleSystemType::geometry)
	{
		if (!m_material->bind2(m_shaderSourceGeometry)) {
			std::cerr << "Failed to bind textures. Skipping draw." << std::endl;
			return;
		}

		m_shaderSourceGeometry.setMat4("model", transformMatrix);
		m_shaderSourceGeometry.setMat4("view", view);
		m_shaderSourceGeometry.setMat4("projection", projection);
		m_shaderSourceGeometry.setFloat("squareSize", m_squareSize);
	}
	else if (m_type == ParticleSystemType::instanced)
	{
		if (!m_material->bind2(m_shaderSourceInstanced)) {
			std::cerr << "Failed to bind textures. Skipping draw." << std::endl;
			return;
		}

		m_shaderSourceInstanced.setMat4("model", transformMatrix);
		m_shaderSourceInstanced.setMat4("view", view);
		m_shaderSourceInstanced.setMat4("projection", projection);
	}

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	
	if (m_type == ParticleSystemType::basic)
	{
		// Send to GPU
		glBindVertexArray(m_VAO);
		OpenGLDebug::checkGLError("glBindVertexArray");

		int numOfSquares = getCurrentDataSize() / 8;
		glDrawElements(GL_TRIANGLES, numOfSquares * 6, GL_UNSIGNED_INT, 0);
		OpenGLDebug::checkGLError("glDrawElements");

		m_drawCallCount++; // Increment for each draw call
	}
	else if (m_type == ParticleSystemType::geometry)
	{
		// Send to GPU
		glBindVertexArray(m_VAO);
		OpenGLDebug::checkGLError("glBindVertexArray");

		glDrawArrays(GL_POINTS, 0, getCurrentDataSize());
		OpenGLDebug::checkGLError("glDrawArrays");

		m_drawCallCount++; // Increment for each draw call
	}
	else if (m_type == ParticleSystemType::instanced)
	{
		// Send to GPU
		glBindVertexArray(m_quadVAO);
		OpenGLDebug::checkGLError("glBindVertexArray");

		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, getCurrentDataSize());
		OpenGLDebug::checkGLError("glDrawArraysInstanced");

		m_drawCallCount++; // Increment for each draw call
	}

	glBindVertexArray(0);
	OpenGLDebug::checkGLError("glBindVertexArray");

	m_material->unbind(); // Unbind textures to prevent OpenGL state retention
	OpenGLDebug::checkGLError("Unbind");


	glDepthMask(GL_TRUE);

	// important !!!
	shader.use();
}

void engine::ParticleSystem::basicDataPrep()
{
	std::vector<glm::vec3>  points = getDataSquarePoints();
	int currentDataSize = getCurrentDataSize();

	glm::vec3* data = points.data();

	int numOfSquares = (currentDataSize / 8);
	std::vector<int> indices;
	for (int i = 0; i < numOfSquares; i++)
	{
		// CCW triangles: (0,1,2) and (2,3,0)
		indices.push_back(i * 4 + 0);
		indices.push_back(i * 4 + 1);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 3);
		indices.push_back(i * 4 + 0);
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

void engine::ParticleSystem::geometryDataPrep()
{
	std::vector<glm::vec3>  points = getDataCenterPoints();
	glm::vec3* data = points.data();
	int currentDataSize = getCurrentDataSize();

	int numOfSquares = (currentDataSize / 8);

	if (currentDataSize > 0) {
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * currentDataSize, data, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glEnableVertexAttribArray(0);
	}
}

void engine::ParticleSystem::instancedDataPrep()
{
	std::vector<glm::vec3> points = getDataCenterPoints();
	glm::vec3* data = points.data();
	int currentDataSize = getCurrentDataSize();

	if (currentDataSize > 0)
	{
		glBindVertexArray(m_quadVAO); // Bind the VAO first

		glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * currentDataSize, data, GL_DYNAMIC_DRAW);

		// Set up the vertex attribute for the instanced data
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glEnableVertexAttribArray(2);
		glVertexAttribDivisor(2, 1); // Tell OpenGL this is an instanced vertex attribute

		// Unbind the VBO, but keep the VAO bound
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void engine::ParticleSystem::destroyParticle(unsigned int index)
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

void engine::ParticleSystem::initParticles(unsigned int n)
{
	float randMax = (float)RAND_MAX;
	
	for (unsigned int i = 0; i < n; i++)
	{
		Particle particle{};
		/*for (unsigned int j = 0; j < m_maxParticles; j++)
		{
			if (m_flags[j] == false) {
				m_particlesArray[j] = particle;
				m_flags[j] = true;
				if (j >= m_maxFilledIndex) {
					m_maxFilledIndex = j;
				}
				break;
			}
		}*/

		// Random angle and distance within the emitter radius
		float theta = rand() / randMax * 2.0f * M_PI;
		float phi = rand() / randMax * 2.0f * M_PI;
		float r = (rand() / randMax) * m_emitterRadius;

		float sinPhi = sin(phi);

		// Convert spherical to Cartesian coordinates
		particle.position.x = r * sinPhi * cos(theta);
		particle.position.y = r * sinPhi * sin(theta);
		particle.position.z = r * cos(phi);

		//float theta = rand() / (float)RAND_MAX * 2.0f * M_PI;
		//float r = (rand() / (float)RAND_MAX) * m_emitterRadius;
		//particle.position.x = r * cos(theta);
		//particle.position.y = r * sin(theta);
		//particle.position.z = 0.0f;

		// ... rest of your particle initialization ...
		for (unsigned int j = 0; j < m_maxParticles; j++) {
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
	float d = m_squareSize;
	// CCW around the quad as seen from +Z (camera side):
	// 0: bottom-left, 1: bottom-right, 2: top-right, 3: top-left
	m_squarePoints[0] = center + glm::vec3(-d, -d, 0); // BL
	m_squarePoints[1] = center + glm::vec3(d, -d, 0); // BR
	m_squarePoints[2] = center + glm::vec3(d, d, 0); // TR
	m_squarePoints[3] = center + glm::vec3(-d, d, 0); // TL
}

int engine::ParticleSystem::getCurrentDataSize()
{
	return m_currentDataSize;
}

std::vector<std::string> engine::ParticleSystem::getModesList()
{
	std::vector<std::string> names;
	names.reserve(3); // avoids reallocation growth

	names.emplace_back("Basic");
	names.emplace_back("Geometry");
	names.emplace_back("Instanced");

	return names;
}

void engine::ParticleSystem::setModeAtIndex(unsigned short index)
{
	if (index == 1)
		m_type = ParticleSystemType::geometry;
	else if (index == 2)
		m_type = ParticleSystemType::instanced;

	m_type = ParticleSystemType::basic;

	reSetup();
}
