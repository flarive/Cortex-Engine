#pragma once

#include "renderer.h"

namespace engine
{
	class VulkanRenderer : public Renderer
	{
	public:
		VulkanRenderer(GLFWwindow* window, RenderMethod method);
		virtual ~VulkanRenderer();

		void renderModel(const std::shared_ptr<Model> model, std::unique_ptr<Shader>& shader, const glm::mat4& transformMatrix, const Transform& localTransform) override;
		void renderMesh(const std::shared_ptr<Mesh> mesh, std::unique_ptr<Shader>& shader, const glm::mat4& transformMatrix) override;
		void renderPrimitive(const std::shared_ptr<Primitive> primitive, std::unique_ptr<Shader>& shader, const glm::mat4& transformMatrix, const Transform& localTransform) override;

		// 2 special cases to get rid of
		void renderParticleSystem(const std::shared_ptr<ParticleSystem> primitive, std::unique_ptr<Shader>& shader, const glm::mat4& transformMatrix, const Transform& localTransform, const glm::mat4& projection, const glm::mat4& view) override;
		void renderTerrain(const std::shared_ptr<Terrain> primitive, std::unique_ptr<Shader>& shader, const glm::mat4& transformMatrix, const Transform& localTransform) override;


		void uploadPrimitive(const std::shared_ptr<Primitive> primitive) override;

	protected:
		void loadShaders() override;

		bool bind(std::unique_ptr<Shader>& shader, const std::vector<std::shared_ptr<Texture>>& textures, int baseUnit = Material::MATERIAL_BASE_UNIT, bool resetHasTextures = true) const override;
		void unbind(const std::vector<std::shared_ptr<Texture>>& textures, int baseUnit = Material::MATERIAL_BASE_UNIT) const override;


	};
}