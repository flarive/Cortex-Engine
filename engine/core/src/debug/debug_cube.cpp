#include "../../include/debug/debug_cube.h"

#include "../../include/singleton.h"

#include <vector>

engine::Shader engine::DebugCube::m_shader; // Define the static member

engine::DebugCube::DebugCube(const glm::vec3& _position)
    : m_position(_position)
{
}

engine::DebugCube::DebugCube(const float& _size, const glm::vec3& _position)
    : m_position(_position), m_width(_size), m_height(_size), m_depth(_size)
{
}

engine::DebugCube::DebugCube(const float& _width, const float& _height, const float& _depth, const glm::vec3& _position)
    : m_position(_position), m_width(_width), m_height(_height), m_depth(_depth)
{
}

void engine::DebugCube::setup()
{
    if (!m_shader.isInitialized()) {
        m_shader.init("model_boundingbox_debug", "shaders/debug/debug_light.vert", "shaders/debug/debug_light.frag");
    }
    
    geometrySetup(); // Geometry setup
}

void engine::DebugCube::geometrySetup()
{
    std::vector<Vertex> vertices = generateVertices();

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    // Send to GPU
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

std::vector<engine::Vertex> engine::DebugCube::generateVertices()
{
    return generateCuboidVertices(m_width, m_height, m_depth, 1.0f);
}

// TODO mutualize !!!!!!!!!!!!!!!!!!!!!
std::vector<engine::Vertex> engine::DebugCube::generateCuboidVertices(float width, float height, float depth, float uvScale)
{
    std::vector<engine::Vertex> vertices{};
    vertices.reserve(36);

    // Half dimensions for easier vertex calculation
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    float halfDepth = depth / 2.0f;

    // Helper lambda to scale UVs
    auto scaleUV = [uvScale](glm::vec2 uv) {
        return glm::vec2{ uv.x * uvScale, uv.y * uvScale };
        };

    // Back face
    vertices.emplace_back(glm::vec3{ -halfWidth, -halfHeight, -halfDepth });
    vertices.emplace_back(glm::vec3{ halfWidth,  halfHeight, -halfDepth });
    vertices.emplace_back(glm::vec3{ halfWidth, -halfHeight, -halfDepth });
    vertices.emplace_back(glm::vec3{ halfWidth,  halfHeight, -halfDepth });
    vertices.emplace_back(glm::vec3{ -halfWidth, -halfHeight, -halfDepth });
    vertices.emplace_back(glm::vec3{ -halfWidth,  halfHeight, -halfDepth });

    // Front face
    vertices.emplace_back(glm::vec3{ -halfWidth, -halfHeight,  halfDepth });
    vertices.emplace_back(glm::vec3{ halfWidth, -halfHeight,  halfDepth });
    vertices.emplace_back(glm::vec3{ halfWidth,  halfHeight,  halfDepth });
    vertices.emplace_back(glm::vec3{ halfWidth,  halfHeight,  halfDepth });
    vertices.emplace_back(glm::vec3{ -halfWidth,  halfHeight,  halfDepth });
    vertices.emplace_back(glm::vec3{ -halfWidth, -halfHeight,  halfDepth });

    // Left face
    vertices.emplace_back(glm::vec3{ -halfWidth,  halfHeight,  halfDepth });
    vertices.emplace_back(glm::vec3{ -halfWidth,  halfHeight, -halfDepth });
    vertices.emplace_back(glm::vec3{ -halfWidth, -halfHeight, -halfDepth });
    vertices.emplace_back(glm::vec3{ -halfWidth, -halfHeight, -halfDepth });
    vertices.emplace_back(glm::vec3{ -halfWidth, -halfHeight,  halfDepth });
    vertices.emplace_back(glm::vec3{ -halfWidth,  halfHeight,  halfDepth });

    // Right face
    vertices.emplace_back(glm::vec3{ halfWidth,  halfHeight,  halfDepth });
    vertices.emplace_back(glm::vec3{ halfWidth, -halfHeight, -halfDepth });
    vertices.emplace_back(glm::vec3{ halfWidth,  halfHeight, -halfDepth });
    vertices.emplace_back(glm::vec3{ halfWidth, -halfHeight, -halfDepth });
    vertices.emplace_back(glm::vec3{ halfWidth,  halfHeight,  halfDepth });
    vertices.emplace_back(glm::vec3{ halfWidth, -halfHeight,  halfDepth });

    // Bottom face
    vertices.emplace_back(glm::vec3{ -halfWidth, -halfHeight, -halfDepth });
    vertices.emplace_back(glm::vec3{ halfWidth, -halfHeight, -halfDepth });
    vertices.emplace_back(glm::vec3{ halfWidth, -halfHeight,  halfDepth });
    vertices.emplace_back(glm::vec3{ halfWidth, -halfHeight,  halfDepth });
    vertices.emplace_back(glm::vec3{ -halfWidth, -halfHeight,  halfDepth });
    vertices.emplace_back(glm::vec3{ -halfWidth, -halfHeight, -halfDepth });

    // Top face
    vertices.emplace_back(glm::vec3{ -halfWidth,  halfHeight, -halfDepth });
    vertices.emplace_back(glm::vec3{ halfWidth,  halfHeight ,  halfDepth });
    vertices.emplace_back(glm::vec3{ halfWidth,  halfHeight, -halfDepth });
    vertices.emplace_back(glm::vec3{ halfWidth,  halfHeight,  halfDepth });
    vertices.emplace_back(glm::vec3{ -halfWidth,  halfHeight, -halfDepth });
    vertices.emplace_back(glm::vec3{ -halfWidth,  halfHeight,  halfDepth });

    return vertices;
}

void engine::DebugCube::draw(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform)
{
    if (!m_shader.isInitialized()) {
        std::cerr << "Shader not valid. Skipping draw." << std::endl;
        return;
    }

    if (!m_shader.isValid()) {
        std::cerr << "Shader program is not valid!" << std::endl;
        return;
    }
    
    m_shader.use();
    OpenGLDebug::checkGLError("shader.use88");
 
    m_shader.setMat4("model", transformMatrix);
    m_shader.setMat4("projection", projection);
    m_shader.setMat4("view", view);
    m_shader.setVec4("customColor", glm::vec4(1.0f, 0.0f, 0.0f, 0.3f));

    // Send to GPU
    glBindVertexArray(m_VAO);
    OpenGLDebug::checkGLError("glBindVertexArray");

    glDrawArrays(GL_TRIANGLES, 0, 36);
    OpenGLDebug::checkGLError("glDrawArrays");

    glBindVertexArray(0);
    OpenGLDebug::checkGLError("glBindVertexArray");

    //glUseProgram(0);
}

void engine::DebugCube::clean()
{
    // delete VAO/VBO
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    
    m_shader.clean();
}