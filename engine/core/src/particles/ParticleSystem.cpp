#include "../../include/particles/particlesystem.h"

#include "../../include/texture.h"

using namespace std;

engine::ParticleSystem::ParticleSystem(int _NMAX, int _numOfParticlesPerSecond) {

	NMAX = _NMAX;
	numOfParticlesPerSecond = _numOfParticlesPerSecond;

	prevTime = glfwGetTime();
	fpsTime = glfwGetTime();

	partCounter = 0;
	nbFrames = 0;
	maxFilledIndex = 0;
	currentDataSize = 0;
	squareSize = 0.25;


	//m_shaderSourceBasic.init("shaderSourceBasic", "shaders/ParticleSourceBasic.vert", "shaders/ParticleSourceBasic.frag");
	//m_shaderSourceGeometry.init("shaderSourceGeometry", "shaders/ParticleSourceGeometry.vert", "shaders/ParticleSourceGeometry.frag", "shaders/ParticleSource.geom");
	//m_shaderSourceInstanced.init("shaderSourceInstanced", "shaders/ParticleSourceInstanced.vert", "shaders/ParticleSourceBasic.frag");


	//texture = Texture::loadTexture("textures/particles/circlealpha.bmp");

	//setUpVertexData();


	particlesArray = new Particle[NMAX];
	flags = new bool[NMAX];

	for (size_t i = 0; i < NMAX; i++)
		flags[i] = false;
}

engine::ParticleSystem::~ParticleSystem() {

	delete(particlesArray);
	delete(flags);
}

//void engine::ParticleSystem::setUpVertexData()
//{
//	// set up vertex data (and buffer(s)) and configure vertex attributes
//	// ------------------------------------------------------------------
//	glGenVertexArrays(1, &VAO);
//	glGenBuffers(1, &VBO);
//	glGenBuffers(1, &EBO);
//
//
//	//if (Global::drawtype == Instanced)
//	//{
//	//	glGenVertexArrays(1, &glData.quadVAO);
//	//	glGenBuffers(1, &glData.quadVBO);
//	//	glBindVertexArray(glData.quadVAO);
//
//	//	float particleSize = Global::particleSize;
//	//	glGenBuffers(1, &glData.instanceVBO);
//
//	//	float quadVertices[] = {
//	//		// positions    
//	//		-particleSize,  particleSize,   0.0f, 1.0f,    // top left
//	//		 particleSize, -particleSize,   1.0f, 0.0f,   // bottom right
//	//		-particleSize, -particleSize,   0.0f, 0.0f,   // bottom left
//
//	//		-particleSize,  particleSize,   0.0f, 1.0f,    // top left
//	//		 particleSize,  -particleSize,  1.0f, 0.0f,   // bottom right
//	//		 particleSize,  particleSize,   1.0f, 1.0f,   // top right 
//	//	};
//	//	glBindBuffer(GL_ARRAY_BUFFER, glData.quadVBO);
//	//	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
//	//	glEnableVertexAttribArray(0);
//	//	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
//	//	glEnableVertexAttribArray(1);
//	//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
//	//}
//}

void engine::ParticleSystem::update()
{
	double currentTime = glfwGetTime();
	double deltaTime = currentTime - prevTime;
	prevTime = currentTime;
	nbFrames++;

	partCounter += ceil(numOfParticlesPerSecond * deltaTime);
	initParticles(partCounter);

	if (currentTime - fpsTime >= 1.0) { 
		//cout << "FPS: " << nbFrames << "\n";
		fpsTime += 1.0;
		nbFrames = 0;
		partCounter = 0;
	}

	for (size_t i = 0; i <= maxFilledIndex; i++)
	{
		if (flags[i] == true) {
			particlesArray[i].lifeSpan -= deltaTime;
			particlesArray[i].position = particlesArray[i].position + particlesArray[i].startVel * glm::vec3(deltaTime);
			if (particlesArray[i].lifeSpan <= 0) {
				destroyParticle(i);
			}
		}
	}
}

void engine::ParticleSystem::destroyParticle(int index)
{
	flags[index] = false;
	if (index == maxFilledIndex) {
		for (int i = maxFilledIndex; i >= 0; i--)
		{
			if (flags[i] == true) {
				maxFilledIndex = i;
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
		for (size_t j = 0; j < NMAX; j++)
		{
			if (flags[j] == false) {
				particlesArray[j] = particle;
				flags[j] = true;
				if (j >= maxFilledIndex) {
					maxFilledIndex = j;
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
	currentDataSize = 0;
	for (size_t i = 0; i <= maxFilledIndex; i++)
	{
		if (flags[i] == true) {
			getSquareFromCenter(particlesArray[i].position);
			for (size_t i = 0; i < 4; i++) {
				points.push_back(squarePoints[i]);
				currentDataSize++;
				if (i == 0) {
					points.push_back(glm::vec3(1, 0, 1));
					currentDataSize++;
				}
				else if (i == 1) {
					points.push_back(glm::vec3(0, 0, 1));
					currentDataSize++;
				}
				else if (i == 2) {
					points.push_back(glm::vec3(0, 1, 1));
					currentDataSize++;
				}
				else if (i == 3) {
					points.push_back(glm::vec3(1, 1, 1));
					currentDataSize++;
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
	currentDataSize = 0;
	for (size_t i = 0; i <= maxFilledIndex; i++)
	{
		if (flags[i] == true) {
			points.push_back(particlesArray[i].position);
			currentDataSize++;
		}
	}
	return points;
}

void engine::ParticleSystem::getSquareFromCenter(glm::vec3 center)
{
	float dist = squareSize;
	//Point down-left
	squarePoints[0] = center + glm::vec3(dist, -dist, 0);
	//Point up-left
	squarePoints[1] = center + glm::vec3(-dist, -dist, 0);
	//Point down-right
	squarePoints[2] = center + glm::vec3(-dist, dist, 0);
	//Point up_right
	squarePoints[3] = center + glm::vec3(dist, dist, 0);
}

int engine::ParticleSystem::getCurrentDataSize()
{
	return currentDataSize;
}

//
//void engine::ParticleSystem::basicDataPrep()
//{
//	std::vector<glm::vec3>  points = getDataSquarePoints();
//	int currentDataSize = getCurrentDataSize();
//
//	glm::vec3* data = points.data();
//
//	int numOfSquares = (currentDataSize / 8);
//	std::vector<int> indices;
//	for (size_t i = 0; i < numOfSquares; i++)
//	{
//		indices.push_back(i * 4);
//		indices.push_back(i * 4 + 1);
//		indices.push_back(i * 4 + 3);
//		indices.push_back(i * 4 + 1);
//		indices.push_back(i * 4 + 2);
//		indices.push_back(i * 4 + 3);
//	}
//	int* indicesData = indices.data();
//
//	if (currentDataSize > 0) {
//		glBindVertexArray(VAO);
//		glBindBuffer(GL_ARRAY_BUFFER, VBO);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * currentDataSize, data, GL_DYNAMIC_DRAW);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numOfSquares * 6 * sizeof(int), indicesData, GL_DYNAMIC_DRAW);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)(sizeof(glm::vec3)));
//		glEnableVertexAttribArray(1);
//
//	}
//
//}
//void engine::ParticleSystem::geometryDataPrep()
//{
//	std::vector<glm::vec3>  points = getDataCenterPoints();
//	glm::vec3* data = points.data();
//	int currentDataSize = getCurrentDataSize();
//
//	int numOfSquares = (currentDataSize / 8);
//
//	if (currentDataSize > 0) {
//		glBindVertexArray(VAO);
//		glBindBuffer(GL_ARRAY_BUFFER, VBO);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * currentDataSize, data, GL_DYNAMIC_DRAW);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
//		glEnableVertexAttribArray(0);
//	}
//
//}
//
//void engine::ParticleSystem::instancedDataPrep()
//{
//	std::vector<glm::vec3>  points = getDataCenterPoints();
//	glm::vec3* data = points.data();
//	int currentDataSize = getCurrentDataSize();
//
//	if (currentDataSize > 0) {
//		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * currentDataSize, data, GL_DYNAMIC_DRAW);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glEnableVertexAttribArray(2);
//		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute comes from a different vertex buffer
//		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.
//	}
//
//}
//
//void engine::ParticleSystem::renderPrep(unsigned int _VAO)
//{
//	glUniformMatrix4fv(id_shader_PVM_uniform, 1, GL_FALSE, glm::value_ptr(PVM));
//	glUniform1f(glGetUniformLocation(m_shaderSourceGeometry.ID, "squareSize"), squareSize); // particle size
//
//	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
//	glClear(GL_COLOR_BUFFER_BIT);
//	glDepthMask(GL_FALSE);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
//
//	glUseProgram(m_shaderSourceGeometry.ID);
//	glBindTexture(GL_TEXTURE_2D, texture);
//	glBindVertexArray(_VAO);
//}
//
//void engine::ParticleSystem::basicRender(int numOfSquares)
//{
//	renderPrep(VAO);
//	glDrawElements(GL_TRIANGLES, numOfSquares * 6, GL_UNSIGNED_INT, 0);
//
//}
//
//void engine::ParticleSystem::geometryRender(int currentDataSize)
//{
//	renderPrep(VAO);
//	glDrawArrays(GL_POINTS, 0, currentDataSize);
//}
//
//void engine::ParticleSystem::instancedRender(int currentDataSize)
//{
//	renderPrep(quadVAO);
//	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, currentDataSize);
//	glBindVertexArray(0);
//}
//
