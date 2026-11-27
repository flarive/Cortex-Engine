#include "../../include/primitives/triangle.h"

#include "../../include/managers/log_manager.h"
#include "../../include/singleton.h"
#include "../../include/uvmapping.h"

#include "../../include/backends/vulkan/vulkan_helper.h"
#include "../../include/app/vulkan_globals.h"

engine::Triangle::Triangle(const glm::vec3& position)
    : Primitive(position)
{
    logger.trace("Triangle constructor called");
}

engine::Triangle::Triangle(float width, float height, const glm::vec3& position)
    : Primitive(position), m_width(width ), m_height(height)
{
    logger.trace("Triangle constructor called");
}

void engine::Triangle::setup()
{
    geometrySetup();
}

void engine::Triangle::setup(const std::shared_ptr<Material>& material)
{
    Primitive::setMaterial(material);

    const UvMapping uv{};
    setup(material, uv);
}

void engine::Triangle::setup(const std::shared_ptr<Material>& material,
    const UvMapping& uv)
{
    Primitive::setMaterial(material);
    m_uvScale = uv.getUvScale();

    geometrySetup();

    if (material && material->hasTextureMap())
        material->loadTexturesAsync(true);
}

void engine::Triangle::geometrySetup()
{
    // === Step 1: Generate vertex data ===
    m_vertices.clear();
    m_indices.clear();

    m_vertices = generateVertices();

    /*AppSettings& appSettings = engine::Singleton::getInstance()->appSettings();
    if (appSettings.backend == BackendType::Vulkan)
    {
        m_vertexCount = static_cast<uint32_t>(vertices.size());

        VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();

        VulkanHelper::createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            m_vertexBuffer,
            m_vertexBufferMemory);

        void* data;
        vkMapMemory(
            VulkanGlobals::getDevice(),
            m_vertexBufferMemory,
            0,
            bufferSize,
            0,
            &data);

        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));

        vkUnmapMemory(
            VulkanGlobals::getDevice(),
            m_vertexBufferMemory);
    }
    else
    {*/
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);

        // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(0);

        // Normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);

        // UV
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
        glEnableVertexAttribArray(2);

        // Tangent
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
        glEnableVertexAttribArray(3);

        // Bitangent
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
        glEnableVertexAttribArray(4);

        glBindVertexArray(0);
    //}
}

std::vector<engine::Vertex> engine::Triangle::generateVertices()
{
    return Primitive::generateTriangleVertices(1.0f, 1.0f, m_uvScale);
}

void engine::Triangle::clean()
{
}

engine::Triangle::~Triangle()
{
    logger.trace("Triangle destructor called");
}
