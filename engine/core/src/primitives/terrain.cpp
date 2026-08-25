#include "../../include/primitives/terrain.h"

#include "../../include/managers/log_manager.h"
#include "../../include/managers/texture_manager.h"

engine::Terrain::Terrain(float heightFactor, unsigned int resolution, const glm::vec3& _position)
	: Primitive(_position), m_heightFactor(heightFactor), m_resolution(resolution)
{
    setIsTessellated(true);

    logger.trace("Terrain constructor called");
}

void engine::Terrain::setup()
{
    const UvMapping uv{};
    setup(nullptr, uv);
}

void engine::Terrain::setup(const std::shared_ptr<Material>& material)
{
    m_material = material; // Store material reference

    const UvMapping uv{};
    setup(material, uv);
}

void engine::Terrain::setup(const std::shared_ptr<Material>& material, const UvMapping& uv)
{
    m_material = material;
    m_uvScale = uv.getUvScale();

    auto allTexturesLoaded = [this](bool) {
        if (this->m_material && this->m_material->hasHeightMap())
        {
            const TextureData* data = TextureManager::getTextureData(this->m_material->getHeightTexPath());
            m_textureWidth = data->width;
            m_textureHeight = data->height;
        }
        else
        {
            m_textureWidth = 1024;
            m_textureHeight = 1024;
        }

        init();
        };

    if (m_material)
    {
        m_material->loadTexturesAsync(true, allTexturesLoaded);
    }
    else
    {
        m_textureWidth = 1024;
        m_textureHeight = 1024;
        init();
    }
}

void engine::Terrain::init()
{
    geometrySetup();
}

void engine::Terrain::geometrySetup()
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
    m_vertices = generateVertices();

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    // Upload full Vertex structs
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(engine::Vertex), m_vertices.data(), GL_STATIC_DRAW);

    GLsizei stride = sizeof(engine::Vertex);

    // --- Position (location = 0) ---
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(engine::Vertex, position));
    glEnableVertexAttribArray(0);

    // --- Normal (location = 1) ---
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(engine::Vertex, normal));
    glEnableVertexAttribArray(1);

    // --- TexCoords (location = 2) ---
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(engine::Vertex, texCoords));
    glEnableVertexAttribArray(2);

    // --- Tangent (location = 3) ---
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(engine::Vertex, tangent));
    glEnableVertexAttribArray(3);

    // --- Bitangent (location = 4) ---
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(engine::Vertex, bitangent));
    glEnableVertexAttribArray(4);


    glPatchParameteri(GL_PATCH_VERTICES, TERRAIN_TESSELLATION_PATCH_COUNT);


    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

std::vector<engine::Vertex> engine::Terrain::generateVertices()
{
    std::vector<engine::Vertex> vertices;
    vertices.reserve(m_resolution * m_resolution * TERRAIN_TESSELLATION_PATCH_COUNT);

    const glm::vec3 normal(0.0f, 1.0f, 0.0f);

    // Tangent space basis (constant for this grid)
    const glm::vec3 tangent(1.0f, 0.0f, 0.0f);
    const glm::vec3 bitangent(0.0f, 0.0f, -1.0f); // flipped V

    int width = m_textureWidth;
    int height = m_textureHeight;

    for (unsigned i = 0; i <= m_resolution - 1; i++)
    {
        for (unsigned j = 0; j <= m_resolution - 1; j++)
        {
            float x0 = -width / 2.0f + width * i / float(m_resolution);
            float x1 = -width / 2.0f + width * (i + 1) / float(m_resolution);
            float z0 = -height / 2.0f + height * j / float(m_resolution);
            float z1 = -height / 2.0f + height * (j + 1) / float(m_resolution);

            float u0 = (i / float(m_resolution)) * m_uvScale;
            float u1 = ((i + 1) / float(m_resolution)) * m_uvScale;
            float v0 = (1.0f - j / float(m_resolution)) * m_uvScale;
            float v1 = (1.0f - (j + 1) / float(m_resolution)) * m_uvScale;

            vertices.emplace_back(glm::vec3(x0, 0.0f, z0), normal, glm::vec2(u0, v0), tangent, bitangent);
            vertices.emplace_back(glm::vec3(x1, 0.0f, z0), normal, glm::vec2(u1, v0), tangent, bitangent);
            vertices.emplace_back(glm::vec3(x0, 0.0f, z1), normal, glm::vec2(u0, v1), tangent, bitangent);
            vertices.emplace_back(glm::vec3(x1, 0.0f, z1), normal, glm::vec2(u1, v1), tangent, bitangent);
        }
    }

    return vertices;
}

void engine::Terrain::draw(engine::Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, engine::Transform& localTransform)
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

    shader.use();
    OpenGLDebug::checkGLError("m_tessHeightMapShader.use");

    setTransform(localTransform.getLocalPosition(), localTransform.getLocalRotation(), localTransform.getLocalScale());

    if (type == ShaderType::BlinnPhongTessellation || type == ShaderType::PBRTessellation)
    {
        if (!m_material->bind(shader)) {
            std::cerr << "Failed to bind textures. Skipping draw." << std::endl;
            return;
        }

        if (type == ShaderType::BlinnPhongTessellation)
        {
            shader.setFloat("material.shininess", m_material->getShininessIntensity());
            shader.setVec3("material.diffuse_color", m_material->getDiffuseColor());
            shader.setVec3("material.specular_color", m_material->getSpecularColor());
        }

        shader.setFloat("material.normalMapIntensity", m_material->getNormalIntensity());
        shader.setFloat("material.emissiveIntensity", m_material->getEmissiveIntensity());

        shader.setBool("material.canCastShadows", canCastShadows());
        shader.setBool("material.canReceiveShadows", canReceiveShadows());


        if (type == ShaderType::PBRTessellation)
        {
            shader.setVec3("material.baseColorFactor", m_material->getBaseColorFactor());
            shader.setVec3("material.ambient_color", m_material->getAmbientColor());
            shader.setFloat("material.ambient_intensity", m_material->getAmbientIntensity());
        }
    }
    else if (type == ShaderType::DepthBufferDirectionalLightsTessellation)
    {
        shader.setInt("materialHeight.texture_height", m_material->getTextureHeightUnit());
        shader.setBool("materialHeight.has_texture_height_map", true);
    }
    
    shader.setMat4("model", transformMatrix);

    if (type == ShaderType::BlinnPhongTessellation || type == ShaderType::PBRTessellation)
    {
        shader.setBool("hasTangents", true);
        shader.setBool("isAnimated", false);
        shader.setBool("isTessellated", isTessellated());
    }

    if (type == ShaderType::BlinnPhongTessellation  || type == ShaderType::PBRTessellation || type == ShaderType::DepthBufferDirectionalLightsTessellation)
    {
        shader.setFloat("heightFactor", m_heightFactor);
        shader.setVec2("heightOffset", m_heightOffset);

        if (type == ShaderType::BlinnPhongTessellation || type == ShaderType::PBRTessellation)
        {
            shader.setFloat("heightTextureSize", static_cast<float>(m_textureWidth));
        }
    }
   

    // render the terrain
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_PATCHES, 0, static_cast<GLsizei>(m_vertices.size()));
    glBindVertexArray(0);

    if (m_material && (type == ShaderType::BlinnPhongTessellation || type == ShaderType::PBRTessellation))
    {
        m_material->unbind(); // Unbind textures to prevent OpenGL state retention
        OpenGLDebug::checkGLError("m_tessHeightMapShader.unbind");
    }
}

void engine::Terrain::clean()
{
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    if (m_VBO != 0) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }

	//m_tessHeightMapShader.clean();
}

engine::Terrain::~Terrain()
{
    logger.trace("Terrain destructor called");
}