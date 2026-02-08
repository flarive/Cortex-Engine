#include "../../include/ecs/particlesystem_component.h"

#include "../../include/singleton.h"

#include "../../include/primitives/primitive.h"


engine::ParticleSystemComponent::ParticleSystemComponent(std::shared_ptr<ParticleSystem> particleSystem) : m_particleSystem(particleSystem)
{
	// Initialize property setters based on primitive type
	m_propertySetters = m_particleSystem->getPropertySetters();
}

void engine::ParticleSystemComponent::init(Transform& transform)
{
	m_particleSystem->setPosition(transform.getLocalPosition());
	m_particleSystem->setRotation(transform.getLocalRotation());
	m_particleSystem->setScale(transform.getLocalScale());



	//Global::setDrawType(0);

	//float squareSize = 0.25f;
	//Global::setParticleSize(squareSize);

	//m_shaderSourceBasic.init("shaderSourceBasic", "shaders/ParticleSourceBasic.vert", "shaders/ParticleSourceBasic.frag");
	//m_shaderSourceGeometry.init("shaderSourceGeometry", "shaders/ParticleSourceGeometry.vert", "shaders/ParticleSourceGeometry.frag", "shaders/ParticleSource.geom");
	//m_shaderSourceInstanced.init("shaderSourceInstanced", "shaders/ParticleSourceInstanced.vert", "shaders/ParticleSourceBasic.frag");


	////select shader sources based on drawtype
	//// --------------------------------------
	//char* vert, * frag, * geom;
	//glHelpers.selectShaders(vert, frag, geom);
	//glData.texture = glHelpers.loadTexture();

	//glHelpers.setUpVertexData(glData);
}

void engine::ParticleSystemComponent::update(float deltaTime, Transform& transform)
{
	m_particleSystem->update();
	
	// recalculated each frame
	m_boundingVolume = std::make_unique<AABB>(generateBoundingVolume(m_particleSystem));

	auto [width, height, depth] = m_boundingVolume->getAABBDimensions();
	m_debug_boundingBox = std::make_unique<DebugCube>(width, height, depth); // Cube at origin with dimensions of the AABB
	m_debug_boundingBox->setup();
}

void engine::ParticleSystemComponent::draw(const glm::mat4& projection, const glm::mat4& view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform, AABB* boundingVolume)
{
	m_particleSystem->draw(shader, projection, view, worldTransformMatrix, localTransform);



	auto* singleton = engine::Singleton::getInstance();
	assert(singleton != nullptr && "Singleton not initialized !");
	SceneSettings& sceneSettings = singleton->sceneSettings();

	if (sceneSettings.drawBoundingBoxesVisualHelpers)
		m_debug_boundingBox->draw(projection, view, worldTransformMatrix, localTransform);


	


	//if (Global::drawtype == Basic) {
	//	glHelpers.basicDataPrep(glData, *m_particleSystem);
	//	int numOfSquares = m_particleSystem->getCurrentDataSize() / 8;
	//	glHelpers.basicRender(glData, numOfSquares);
	//}
	//else if (Global::drawtype == Geometry) {
	//	glHelpers.geometryDataPrep(glData, *m_particleSystem);
	//	glHelpers.geometryRender(glData, m_particleSystem->getCurrentDataSize());
	//}
	//else if (Global::drawtype == Instanced) {
	//	glHelpers.instancedDataPrep(glData, *m_particleSystem);
	//	glHelpers.instancedRender(glData, m_particleSystem->getCurrentDataSize());
	//}
}

engine::AABB engine::ParticleSystemComponent::generateBoundingVolume(const std::shared_ptr<ParticleSystem> particleSystem)
{
	glm::vec3 minV(std::numeric_limits<float>::max());
	glm::vec3 maxV(-std::numeric_limits<float>::max());

	

	// Conservative radius for a square billboard (half diagonal)
	const float r = particleSystem->getSquareSize() * 1.41421356237f; // sqrt(2)
	const glm::vec3 e(r, r, r);

	bool any = false;

	for (size_t i = 0; i <= particleSystem->getMaxFilledIndex(); ++i)
	{
		if (!particleSystem->getFlags()[i]) continue;

		const glm::vec3 p = particleSystem->getParticleArray()[i].position;
		minV = glm::min(minV, p - e);
		maxV = glm::max(maxV, p + e);
		any = true;
	}

	// If no particles alive: return a degenerate box (or mark invalid)
	if (!any)
	{
		// Choose what makes sense in your engine:
		// e.g. center at origin, extents 0.
		return engine::AABB(glm::vec3(0.0f), glm::vec3(0.0f));
	}

	return engine::AABB(minV, maxV);
}

engine::AABB* engine::ParticleSystemComponent::getBoundingVolume()
{
	return m_boundingVolume.get();
}

engine::ordered_map<std::string, engine::EditorProperty> engine::ParticleSystemComponent::getPublicProperties()
{
	return m_particleSystem->getPublicProperties();
}

std::unordered_map<std::string, std::function<void(engine::EditorPropertyValue)>> engine::ParticleSystemComponent::getPropertySetters()
{
	return m_particleSystem->getPropertySetters();
}

void engine::ParticleSystemComponent::setProperty(const std::string& key, engine::EditorPropertyValue value)
{
	auto it = m_propertySetters.find(key);
	if (it != m_propertySetters.end())
	{
		it->second(value);
		m_particleSystem->reSetup(); // Assuming all primitives have a reSetup() method
	}
}

void engine::ParticleSystemComponent::setEnabled(bool enabled)
{
	ComponentBase<ParticleSystemComponent>::setEnabled(enabled);

	m_particleSystem->setEnabled(enabled);
}
