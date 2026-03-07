#include "../../include/terrain/terrain.h"

engine::Terrain::Terrain(float sizeFactor, unsigned int patchCount, unsigned int resolution)
	: m_patchCount(patchCount), m_resolution(resolution), m_sizeFactor(sizeFactor)
{
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
    m_material = material; // Store material reference
    m_uvScale = uv.getUvScale();

    loadShaders();

    auto allTexturesLoaded = [this](bool) {
        TextureData data = Texture::getTextureData(this->m_material->getHeightTexPath());
		m_textureWidth = std::get<2>(data);
        m_textureHeight = std::get<3>(data);

        init();
    };

    if (m_material && m_material->hasDiffuseMap())
        m_material->loadTexturesAsync(allTexturesLoaded);
}

void engine::Terrain::init()
{
    geometrySetup();
}

void engine::Terrain::loadShaders()
{
    m_tessHeightMapShader.init("height", "shaders/height.vert", "shaders/height.frag", nullptr, "shaders/height.tcs", "shaders/height.tes");

    

    // to remove !!!!
    //engine::TextureData data = Texture::loadTextureExtended("textures/height/iceland_heightmap.png", true, false);
    //m_textureWidth = std::get<2>(data) / 3;
    //m_textureHeight = std::get<3>(data) / 3;
}

void engine::Terrain::geometrySetup()
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
    m_vertices = generateVertices();

    glGenVertexArrays(1, &m_terrainVAO);
    glBindVertexArray(m_terrainVAO);

    glGenBuffers(1, &m_terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_terrainVBO);

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


    glPatchParameteri(GL_PATCH_VERTICES, m_patchCount);


    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

std::vector<engine::Vertex> engine::Terrain::generateVertices()
{
    std::vector<engine::Vertex> vertices;
    vertices.reserve(m_resolution * m_resolution * m_patchCount); // 4 vertices per quad

    const glm::vec3 normal(0.0f, 1.0f, 0.0f);


	int width = static_cast<int>(m_textureWidth * m_sizeFactor);
    int height = static_cast<int>(m_textureHeight * m_sizeFactor);

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
            float v0 = (j / float(m_resolution)) * m_uvScale;
            float v1 = ((j + 1) / float(m_resolution)) * m_uvScale;


            // Vertex 1
            vertices.emplace_back(
                glm::vec3(x0, 0.0f, z0),
                normal,
                glm::vec2(u0, v0)
            );

            // Vertex 2
            vertices.emplace_back(
                glm::vec3(x1, 0.0f, z0),
                normal,
                glm::vec2(u1, v0)
            );

            // Vertex 3
            vertices.emplace_back(
                glm::vec3(x0, 0.0f, z1),
                normal,
                glm::vec2(u0, v1)
            );

            // Vertex 4
            vertices.emplace_back(
                glm::vec3(x1, 0.0f, z1),
                normal,
                glm::vec2(u1, v1)
            );
        }
    }

    return vertices;
}


void engine::Terrain::draw(engine::Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, engine::Transform& localTransform)
{
    if (!m_isEnabled)
        return;

    if (!m_material || !m_tessHeightMapShader.isValid()) {
        std::cerr << "Material or shader not valid. Skipping draw." << std::endl;
        return;
    }

    if (!m_material->areAllTexturesLoaded()) {
        std::cout << "Textures not ready. Deferring draw." << std::endl;
        return;
    }

    if (m_terrainVAO == 0 || m_terrainVBO == 0) {
        std::cerr << "VAO/VBO not initialized. Skipping draw." << std::endl;
        return;
    }

    m_tessHeightMapShader.use();
    OpenGLDebug::checkGLError("m_tessHeightMapShader.use");

    setTransform(localTransform.getLocalPosition(), localTransform.getLocalRotation(), localTransform.getLocalScale());

    if (!m_material->bind(m_tessHeightMapShader)) {
        std::cerr << "Failed to bind textures. Skipping draw." << std::endl;
        return;
    }
    OpenGLDebug::checkGLError("m_tessHeightMapShader.bind");

    
    m_tessHeightMapShader.setMat4("projection", projection);
    m_tessHeightMapShader.setMat4("view", view);
    m_tessHeightMapShader.setMat4("model", transformMatrix);


    m_tessHeightMapShader.setVec3("light.position", glm::vec3(0.0f, 100.0f, 0.0f));
    m_tessHeightMapShader.setVec3("light.ambient", 0.3f, 0.3f, 0.3f);  // Increase ambient light
    m_tessHeightMapShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);  // Increase diffuse light
    m_tessHeightMapShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f); // Increase specular light

    // render the terrain
    glBindVertexArray(m_terrainVAO);
    glDrawArrays(GL_PATCHES, 0, static_cast<GLsizei>(m_vertices.size()));
    glBindVertexArray(0);


    m_material->unbind(); // Unbind textures to prevent OpenGL state retention
    OpenGLDebug::checkGLError("m_tessHeightMapShader.unbind");

    shader.use();
}

void engine::Terrain::clean()
{
    if (m_terrainVAO != 0) {
        glDeleteVertexArrays(1, &m_terrainVAO);
        m_terrainVAO = 0;
    }
    if (m_terrainVBO != 0) {
        glDeleteBuffers(1, &m_terrainVBO);
        m_terrainVBO = 0;
    }

	m_tessHeightMapShader.clean();
}
