#include "../../include/ecs/model_component.h"

#include "../../include/singleton.h"



engine::ModelComponent::ModelComponent(std::shared_ptr<Model> model)
    : m_model(model)
{
	m_boundingVolume = std::make_unique<AABB>(generateBoundingVolume(model));

	// load light cube debug shader
	m_lightDebugShader.init("model_boundingbox_debug", "shaders/debug/debug_light.vert", "shaders/debug/debug_light.frag");

	auto matDebugLight = std::make_shared<engine::Material>(engine::Color(1.0f, 0.0f, 0.0f, 0.5f));

	auto [width, height, depth] = m_boundingVolume->getAABBDimensions();
	m_debug_boundingBox = std::make_unique<Cube>(width, height, depth); // Cube at origin with dimensions of the AABB
	m_debug_boundingBox->setup(matDebugLight);
}


void engine::ModelComponent::init(Transform& transform)
{
	m_model->position = transform.getLocalPosition();
	m_model->rotation = transform.getLocalRotation();
	m_model->scale = transform.getLocalScale();
}

void engine::ModelComponent::update(Transform& transform)
{

}

void engine::ModelComponent::draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform, AABB* boundingVolume)
{
    m_model->draw(shader, worldTransformMatrix, localTransform);

	auto* singleton = engine::Singleton::getInstance();
	assert(singleton != nullptr && "Singleton not initialized !");
	SceneSettings& sceneSettings = singleton->sceneSettings();

	if (sceneSettings.drawBoundingBoxesVisualHelpers)
	{
		// Pass model matrix to shader
		m_lightDebugShader.use();
		m_lightDebugShader.setMat4("projection", projection);
		m_lightDebugShader.setMat4("view", view);
		m_lightDebugShader.setVec4("customColor", m_debug_boundingBox->getMaterial()->getAmbientColor());
		m_debug_boundingBox->draw(m_lightDebugShader, projection, view, worldTransformMatrix, localTransform);
	}
}

engine::AABB engine::ModelComponent::generateBoundingVolume(const std::shared_ptr<Model> model)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

	if (model)
	{
		for (auto&& mesh : model->meshes)
		{
			for (auto&& vertex : mesh.vertices)
			{
				minAABB.x = std::min(minAABB.x, vertex.position.x);
				minAABB.y = std::min(minAABB.y, vertex.position.y);
				minAABB.z = std::min(minAABB.z, vertex.position.z);

				maxAABB.x = std::max(maxAABB.x, vertex.position.x);
				maxAABB.y = std::max(maxAABB.y, vertex.position.y);
				maxAABB.z = std::max(maxAABB.z, vertex.position.z);
			}
		}
	}
	return engine::AABB(minAABB, maxAABB);
}

engine::AABB* engine::ModelComponent::getBoundingVolume()
{
	return m_boundingVolume.get();
}

engine::ordered_map<std::string, std::variant<int, std::string, float, bool>> engine::ModelComponent::getPublicProperties()
{
	return engine::ordered_map<std::string, std::variant<int, std::string, float, bool>>{};
}

std::unordered_map<std::string, std::function<void(float)>> engine::ModelComponent::getPropertySetters()
{
	return std::unordered_map<std::string, std::function<void(float)>>();
}
