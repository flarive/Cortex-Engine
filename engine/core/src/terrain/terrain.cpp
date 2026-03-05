#include "../../include/terrain/terrain.h"

engine::Terrain::Terrain(unsigned int width, unsigned int height, unsigned int patchCount)
    : m_width(width), m_height(height), m_patchCount(patchCount)
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

    geometrySetup();
}

void engine::Terrain::loadShaders()
{
    m_tessHeightMapShader.init("height", "shaders/height.vert", "shaders/height.frag", nullptr, "shaders/height.tcs", "shaders/height.tes");


    engine::TextureData data = Texture::loadTextureExtended("textures/height/iceland_heightmap.png", true, false);
    m_textureId = std::get<0>(data);
    m_textureWidth = std::get<2>(data);
    m_textureHeight = std::get<3>(data);

    //if (m_textureId > 0)
    //{
    //    m_tessHeightMapShader.use();
    //    glActiveTexture(GL_TEXTURE0 + 18);
    //    glBindTexture(GL_TEXTURE_2D, m_textureId);
    //    m_tessHeightMapShader.setInt("heightMap", 18);
    //}
}

void engine::Terrain::geometrySetup()
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
    
    std::vector<engine::Vertex> vertices = generateVertices();

    glGenVertexArrays(1, &m_terrainVAO);
    glBindVertexArray(m_terrainVAO);

    glGenBuffers(1, &m_terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_terrainVBO);

    // Upload full Vertex structs
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(engine::Vertex), vertices.data(), GL_STATIC_DRAW);

    GLsizei stride = sizeof(engine::Vertex);

    // --- Position (location = 0) ---
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(engine::Vertex, position));
    glEnableVertexAttribArray(0);

    // --- TexCoords (location = 1) ---
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(engine::Vertex, texCoords));
    glEnableVertexAttribArray(1);


    glPatchParameteri(GL_PATCH_VERTICES, m_patchCount);
}

std::vector<engine::Vertex> engine::Terrain::generateVertices()
{
    std::vector<engine::Vertex> vertices;
    vertices.reserve(m_rez * m_rez * 4); // 4 vertices per quad

    const glm::vec3 normal(0.0f, 1.0f, 0.0f);

    for (unsigned i = 0; i <= m_rez - 1; i++)
    {
        for (unsigned j = 0; j <= m_rez - 1; j++)
        {
            float x0 = -m_textureWidth / 2.0f + m_textureWidth * i / float(m_rez);
            float x1 = -m_textureWidth / 2.0f + m_textureWidth * (i + 1) / float(m_rez);
            float z0 = -m_textureHeight / 2.0f + m_textureHeight * j / float(m_rez);
            float z1 = -m_textureHeight / 2.0f + m_textureHeight * (j + 1) / float(m_rez);

            float u0 = i / float(m_rez);
            float u1 = (i + 1) / float(m_rez);
            float v0 = j / float(m_rez);
            float v1 = (j + 1) / float(m_rez);

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

    if (!m_material || !shader.isValid()) {
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

    /*if (!m_material->bind(shader)) {
        std::cerr << "Failed to bind textures. Skipping draw." << std::endl;
        return;
    }*/

    // view/projection transformations
    //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
    //glm::mat4 view = camera.GetViewMatrix();
    m_tessHeightMapShader.setMat4("projection", projection);
    m_tessHeightMapShader.setMat4("view", view);

    // world transformation
    m_tessHeightMapShader.setMat4("model", transformMatrix);


    // world transformation
    //glm::mat4 model = glm::mat4(1.0f);
    //m_tessHeightMapShader.setMat4("model", model);

    if (m_textureId > 0)
    {
        //m_tessHeightMapShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textureId);
        m_tessHeightMapShader.setInt("heightMap", 0);
    }

    // render the terrain
    glBindVertexArray(m_terrainVAO);
    glDrawArrays(GL_PATCHES, 0, m_patchCount * m_rez * m_rez);
    glBindVertexArray(0);

    //glBindTexture(GL_TEXTURE_2D, 0);

    shader.use();
}

void engine::Terrain::clean()
{
    // TODO
}
