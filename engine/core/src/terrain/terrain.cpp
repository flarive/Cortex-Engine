#include "../../include/terrain/terrain.h"

void engine::Terrain::setup()
{
}

void engine::Terrain::setup(const std::shared_ptr<Terrain>& material)
{
}

void engine::Terrain::setup(const std::shared_ptr<Terrain>& material, const UvMapping& uv)
{
}

void engine::Terrain::draw(engine::Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, engine::Transform& localTransform)
{

}

std::vector<engine::Vertex> engine::Terrain::generateVertices()
{
	return std::vector<engine::Vertex>{};
}

void engine::Terrain::clean()
{

}

void engine::Terrain::geometrySetup()
{

}