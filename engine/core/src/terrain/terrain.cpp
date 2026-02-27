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


    engine::TextureData data = Texture::loadTextureExtended("textures/height/iceland_heightmap.png", true, false);
    m_textureId = std::get<0>(data);
    m_textureWidth = std::get<2>(data);
    m_textureHeight = std::get<3>(data);


    loadShaders();

    geometrySetup();
}

void engine::Terrain::loadShaders()
{
    m_tessHeightMapShader.init("height", "shaders/height.vert", "shaders/height.frag", nullptr, "shaders/height.tcs", "shaders/height.tes");

    if (m_textureId > 0)
        m_tessHeightMapShader.setInt("heightMap", m_textureId);
}

void engine::Terrain::geometrySetup()
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    //std::vector<float> vertices;

    //
    //for (unsigned i = 0; i <= m_rez - 1; i++)
    //{
    //    for (unsigned j = 0; j <= m_rez - 1; j++)
    //    {
    //        vertices.push_back(-m_width / 2.0f + m_width * i / (float)m_rez); // v.x
    //        vertices.push_back(0.0f); // v.y
    //        vertices.push_back(-m_height / 2.0f + m_height * j / (float)m_rez); // v.z
    //        vertices.push_back(i / (float)m_rez); // u
    //        vertices.push_back(j / (float)m_rez); // v

    //        vertices.push_back(-m_width / 2.0f + m_width * (i + 1) / (float)m_rez); // v.x
    //        vertices.push_back(0.0f); // v.y
    //        vertices.push_back(-m_height / 2.0f + m_height * j / (float)m_rez); // v.z
    //        vertices.push_back((i + 1) / (float)m_rez); // u
    //        vertices.push_back(j / (float)m_rez); // v

    //        vertices.push_back(-m_width / 2.0f + m_width * i / (float)m_rez); // v.x
    //        vertices.push_back(0.0f); // v.y
    //        vertices.push_back(-m_height / 2.0f + m_height * (j + 1) / (float)m_rez); // v.z
    //        vertices.push_back(i / (float)m_rez); // u
    //        vertices.push_back((j + 1) / (float)m_rez); // v

    //        vertices.push_back(-m_width / 2.0f + m_width * (i + 1) / (float)m_rez); // v.x
    //        vertices.push_back(0.0f); // v.y
    //        vertices.push_back(-m_height / 2.0f + m_height * (j + 1) / (float)m_rez); // v.z
    //        vertices.push_back((i + 1) / (float)m_rez); // u
    //        vertices.push_back((j + 1) / (float)m_rez); // v
    //    }
    //}
    //std::cout << "Loaded " << m_rez * m_rez << " patches of 4 control points each" << std::endl;
    //std::cout << "Processing " << m_rez * m_rez * 4 << " vertices in vertex shader" << std::endl;


    //// first, configure the cube's VAO (and terrainVBO)
    //glGenVertexArrays(1, &m_terrainVAO);
    //glBindVertexArray(m_terrainVAO);

    //glGenBuffers(1, &m_terrainVBO);
    //glBindBuffer(GL_ARRAY_BUFFER, m_terrainVBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    //// position attribute
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);
    //// texCoord attribute
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
    //glEnableVertexAttribArray(1);

    //glPatchParameteri(GL_PATCH_VERTICES, m_patchCount);


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




    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(engine::Vertex, normal)
    //);

    //// --- TexCoords (location = 2) ---
    //glEnableVertexAttribArray(2);
    //glVertexAttribPointer(
    //    2,
    //    2, GL_FLOAT, GL_FALSE,
    //    stride,
    //    (void*)offsetof(engine::Vertex, texCoords)
    //);

    //// --- Tangent (location = 3) ---
    //glEnableVertexAttribArray(3);
    //glVertexAttribPointer(
    //    3,
    //    3, GL_FLOAT, GL_FALSE,
    //    stride,
    //    (void*)offsetof(engine::Vertex, tangent)
    //);

    //// --- Bitangent (location = 4) ---
    //glEnableVertexAttribArray(4);
    //glVertexAttribPointer(
    //    4,
    //    3, GL_FLOAT, GL_FALSE,
    //    stride,
    //    (void*)offsetof(engine::Vertex, bitangent)
    //);

    //// Bone IDs (location = 5)
    //glEnableVertexAttribArray(5);
    //glVertexAttribIPointer(
    //    5,
    //    MAX_BONE_INFLUENCE, GL_INT,
    //    stride,
    //    (void*)offsetof(engine::Vertex, boneIDs)
    //);

    //// Bone weights (location = 6)
    //glEnableVertexAttribArray(6);
    //glVertexAttribPointer(
    //    6,
    //    MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE,
    //    stride,
    //    (void*)offsetof(engine::Vertex, weights)
    //);

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
    // be sure to activate shader when setting uniforms/drawing objects
    m_tessHeightMapShader.use();

    // view/projection transformations
    //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
    //glm::mat4 view = camera.GetViewMatrix();
    m_tessHeightMapShader.setMat4("projection", projection);
    m_tessHeightMapShader.setMat4("view", view);

    // world transformation
    m_tessHeightMapShader.setMat4("model", transformMatrix);

    // render the terrain
    glBindVertexArray(m_terrainVAO);
    glDrawArrays(GL_PATCHES, 0, m_patchCount * m_rez * m_rez);
}

void engine::Terrain::clean()
{
    // TODO
}
