#include "../../include/primitives/primitive.h"

#include "../../include/singleton.h"
#include "../../include/managers/log_manager.h"

engine::Primitive::Primitive(const glm::vec3& _position) : m_position(_position)
{
}

void engine::Primitive::setMaterial(const std::shared_ptr<Material>& material)
{
    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    SceneSettings& sceneSettings = singleton->sceneSettings();

    auto materialType = material->getTypeID();
    if (sceneSettings.method == RenderMethod::Phong && materialType != MaterialType::blinnphong) {
        logger.warn("BlinnPhong material expected when using Phong renderer !");
    }
    else if (sceneSettings.method == RenderMethod::BlinnPhong && materialType != MaterialType::blinnphong) {
        logger.warn("BlinnPhong material expected when using BlinnPhong renderer !");
    }
    else if (sceneSettings.method == RenderMethod::PBR && materialType != MaterialType::PBR) {
        logger.warn("PBR material expected when using PBR renderer !");
    }

    m_material = material;
}

void engine::Primitive::clean()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);

    m_VAO = 0;
    m_VBO = 0;
    m_EBO = 0;
}

std::vector<engine::Vertex> engine::Primitive::generatePlaneVertices(float uvScale, bool flipNormal)
{
    std::vector<engine::Vertex> vertices;

    // Define positions (XZ plane, facing +Y)
    glm::vec3 pos1(-1.0f, 0.0f, -1.0f); // Bottom-left
    glm::vec3 pos2(1.0f, 0.0f, -1.0f);  // Bottom-right
    glm::vec3 pos3(1.0f, 0.0f, 1.0f);   // Top-right
    glm::vec3 pos4(-1.0f, 0.0f, 1.0f);  // Top-left

    // Texture coordinates (standard UV mapping)
    glm::vec2 uv1(0.0f, 0.0f);
    glm::vec2 uv2(uvScale, 0.0f);
    glm::vec2 uv3(uvScale, uvScale);
    glm::vec2 uv4(0.0f, uvScale);

    // Normal vector (facing up)
    glm::vec3 normal = flipNormal ? glm::vec3(0.0f, -1.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);

    // --- Compute tangent and bitangent (once for the plane) ---
    glm::vec3 edge1 = pos2 - pos1;
    glm::vec3 edge2 = pos4 - pos1;
    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv4 - uv1;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    glm::vec3 tangent, bitangent;
    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent = glm::normalize(tangent);

    bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent = glm::normalize(bitangent);

    if (flipNormal) {
        tangent = -tangent;
        bitangent = -bitangent;
    }

    // CCW winding for upward (+Y) facing plane
    vertices.emplace_back(pos1, normal, uv1, tangent, bitangent);
    vertices.emplace_back(pos4, normal, uv4, tangent, bitangent);
    vertices.emplace_back(pos2, normal, uv2, tangent, bitangent);

    vertices.emplace_back(pos2, normal, uv2, tangent, bitangent);
    vertices.emplace_back(pos4, normal, uv4, tangent, bitangent);
    vertices.emplace_back(pos3, normal, uv3, tangent, bitangent);

    return vertices;
}

std::vector<engine::Vertex> engine::Primitive::generateBillboardVertices(float uvScale)
{
    // force rotation to have a vertical billboard
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f),
        glm::radians(0.0f),
        glm::vec3(0, 0, 1));

    auto R = [&](const glm::vec3& v)
        {
            return glm::vec3(rot * glm::vec4(v, 1.0f));
        };

    glm::vec3 normal = R({ 0, 0, 1 });
    glm::vec3 tangent = R({ 1, 0, 0 });
    glm::vec3 bitangent = R({ 0, 1, 0 });

    // Quad corners (will be rotated)
    glm::vec3 positions[] = {
        R({ -0.5f, -0.5f, 0.0f }), // bottom-left
        R({  0.5f, -0.5f, 0.0f }), // bottom-right
        R({  0.5f,  0.5f, 0.0f }), // top-right
        R({ -0.5f,  0.5f, 0.0f })  // top-left
    };

    // UVs (UVs themselves do NOT rotate under geometric rotation)
    glm::vec2 uvs[] = {
        { 0.0f, 0.0f },
        { uvScale, 0.0f },
        { uvScale, uvScale },
        { 0.0f, uvScale }
    };

    // Build triangles
    std::vector<engine::Vertex> vertices;
    vertices.reserve(6);

    vertices.emplace_back(positions[0], normal, uvs[0], tangent, bitangent);
    vertices.emplace_back(positions[1], normal, uvs[1], tangent, bitangent);
    vertices.emplace_back(positions[2], normal, uvs[2], tangent, bitangent);

    vertices.emplace_back(positions[0], normal, uvs[0], tangent, bitangent);
    vertices.emplace_back(positions[2], normal, uvs[2], tangent, bitangent);
    vertices.emplace_back(positions[3], normal, uvs[3], tangent, bitangent);

    return vertices;
}

std::vector<engine::Vertex> engine::Primitive::generateCubeVertices(float uvScale)
{
    std::vector<engine::Vertex> vertices{};
    vertices.reserve(36);

    // Helper lambda to scale UVs
    auto scaleUV = [uvScale](glm::vec2 uv) {
        return glm::vec2{ uv.x * uvScale, uv.y * uvScale };
        };

    // Back face
    vertices.emplace_back(glm::vec3{ -1.0f, -1.0f, -1.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f }, scaleUV(glm::vec2{ 0.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ 1.0f,  1.0f, -1.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f }, scaleUV(glm::vec2{ 1.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ 1.0f, -1.0f, -1.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f }, scaleUV(glm::vec2{ 1.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ 1.0f,  1.0f, -1.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f }, scaleUV(glm::vec2{ 1.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ -1.0f, -1.0f, -1.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f }, scaleUV(glm::vec2{ 0.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ -1.0f,  1.0f, -1.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f }, scaleUV(glm::vec2{ 0.0f, 1.0f }));

    // Front face
    vertices.emplace_back(glm::vec3{ -1.0f, -1.0f,  1.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, scaleUV(glm::vec2{ 0.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ 1.0f, -1.0f,  1.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, scaleUV(glm::vec2{ 1.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ 1.0f,  1.0f,  1.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, scaleUV(glm::vec2{ 1.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ 1.0f,  1.0f,  1.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, scaleUV(glm::vec2{ 1.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ -1.0f,  1.0f,  1.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, scaleUV(glm::vec2{ 0.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ -1.0f, -1.0f,  1.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, scaleUV(glm::vec2{ 0.0f, 0.0f }));

    // Left face
    vertices.emplace_back(glm::vec3{ -1.0f,  1.0f,  1.0f }, glm::vec3{ -1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ -1.0f,  1.0f, -1.0f }, glm::vec3{ -1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ -1.0f, -1.0f, -1.0f }, glm::vec3{ -1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ -1.0f, -1.0f, -1.0f }, glm::vec3{ -1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ -1.0f, -1.0f,  1.0f }, glm::vec3{ -1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ -1.0f,  1.0f,  1.0f }, glm::vec3{ -1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 0.0f }));

    // Right face
    vertices.emplace_back(glm::vec3{ 1.0f,  1.0f,  1.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ 1.0f, -1.0f, -1.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ 1.0f,  1.0f, -1.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ 1.0f, -1.0f, -1.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ 1.0f,  1.0f,  1.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ 1.0f, -1.0f,  1.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 0.0f }));

    // Bottom face
    vertices.emplace_back(glm::vec3{ -1.0f, -1.0f, -1.0f }, glm::vec3{ 0.0f, -1.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ 1.0f, -1.0f, -1.0f }, glm::vec3{ 0.0f, -1.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ 1.0f, -1.0f,  1.0f }, glm::vec3{ 0.0f, -1.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ 1.0f, -1.0f,  1.0f }, glm::vec3{ 0.0f, -1.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ -1.0f, -1.0f,  1.0f }, glm::vec3{ 0.0f, -1.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ -1.0f, -1.0f, -1.0f }, glm::vec3{ 0.0f, -1.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 1.0f }));

    // Top face
    vertices.emplace_back(glm::vec3{ -1.0f,  1.0f, -1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ 1.0f,  1.0f , 1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ 1.0f,  1.0f, -1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ 1.0f,  1.0f,  1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ -1.0f,  1.0f, -1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ -1.0f,  1.0f,  1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 0.0f }));

    // Compute tangents and bitangents
    for (size_t i = 0; i < vertices.size(); i += 3) {
        glm::vec3 edge1 = vertices[i + 1].position - vertices[i].position;
        glm::vec3 edge2 = vertices[i + 2].position - vertices[i].position;
        glm::vec2 deltaUV1 = vertices[i + 1].texCoords - vertices[i].texCoords;
        glm::vec2 deltaUV2 = vertices[i + 2].texCoords - vertices[i].texCoords;
        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        glm::vec3 tangent;
        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent = glm::normalize(tangent);
        glm::vec3 bitangent;
        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent = glm::normalize(bitangent);
        vertices[i].tangent = tangent;
        vertices[i + 1].tangent = tangent;
        vertices[i + 2].tangent = tangent;
        vertices[i].bitangent = bitangent;
        vertices[i + 1].bitangent = bitangent;
        vertices[i + 2].bitangent = bitangent;
    }

    return vertices;
}

std::vector<engine::Vertex> engine::Primitive::generateCuboidVertices(float width, float height, float depth, float uvScale)
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
    vertices.emplace_back(glm::vec3{ -halfWidth, -halfHeight, -halfDepth }, glm::vec3{ 0.0f, 0.0f, -1.0f }, scaleUV(glm::vec2{ 0.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ halfWidth,  halfHeight, -halfDepth }, glm::vec3{ 0.0f, 0.0f, -1.0f }, scaleUV(glm::vec2{ 1.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ halfWidth, -halfHeight, -halfDepth }, glm::vec3{ 0.0f, 0.0f, -1.0f }, scaleUV(glm::vec2{ 1.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ halfWidth,  halfHeight, -halfDepth }, glm::vec3{ 0.0f, 0.0f, -1.0f }, scaleUV(glm::vec2{ 1.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ -halfWidth, -halfHeight, -halfDepth }, glm::vec3{ 0.0f, 0.0f, -1.0f }, scaleUV(glm::vec2{ 0.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ -halfWidth,  halfHeight, -halfDepth }, glm::vec3{ 0.0f, 0.0f, -1.0f }, scaleUV(glm::vec2{ 0.0f, 1.0f }));

    // Front face
    vertices.emplace_back(glm::vec3{ -halfWidth, -halfHeight,  halfDepth }, glm::vec3{ 0.0f, 0.0f, 1.0f }, scaleUV(glm::vec2{ 0.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ halfWidth, -halfHeight,  halfDepth }, glm::vec3{ 0.0f, 0.0f, 1.0f }, scaleUV(glm::vec2{ 1.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ halfWidth,  halfHeight,  halfDepth }, glm::vec3{ 0.0f, 0.0f, 1.0f }, scaleUV(glm::vec2{ 1.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ halfWidth,  halfHeight,  halfDepth }, glm::vec3{ 0.0f, 0.0f, 1.0f }, scaleUV(glm::vec2{ 1.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ -halfWidth,  halfHeight,  halfDepth }, glm::vec3{ 0.0f, 0.0f, 1.0f }, scaleUV(glm::vec2{ 0.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ -halfWidth, -halfHeight,  halfDepth }, glm::vec3{ 0.0f, 0.0f, 1.0f }, scaleUV(glm::vec2{ 0.0f, 0.0f }));

    // Left face
    vertices.emplace_back(glm::vec3{ -halfWidth,  halfHeight,  halfDepth }, glm::vec3{ -1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ -halfWidth,  halfHeight, -halfDepth }, glm::vec3{ -1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ -halfWidth, -halfHeight, -halfDepth }, glm::vec3{ -1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ -halfWidth, -halfHeight, -halfDepth }, glm::vec3{ -1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ -halfWidth, -halfHeight,  halfDepth }, glm::vec3{ -1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ -halfWidth,  halfHeight,  halfDepth }, glm::vec3{ -1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 0.0f }));

    // Right face
    vertices.emplace_back(glm::vec3{ halfWidth,  halfHeight,  halfDepth }, glm::vec3{ 1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ halfWidth, -halfHeight, -halfDepth }, glm::vec3{ 1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ halfWidth,  halfHeight, -halfDepth }, glm::vec3{ 1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ halfWidth, -halfHeight, -halfDepth }, glm::vec3{ 1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ halfWidth,  halfHeight,  halfDepth }, glm::vec3{ 1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ halfWidth, -halfHeight,  halfDepth }, glm::vec3{ 1.0f, 0.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 0.0f }));

    // Bottom face
    vertices.emplace_back(glm::vec3{ -halfWidth, -halfHeight, -halfDepth }, glm::vec3{ 0.0f, -1.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ halfWidth, -halfHeight, -halfDepth }, glm::vec3{ 0.0f, -1.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ halfWidth, -halfHeight,  halfDepth }, glm::vec3{ 0.0f, -1.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ halfWidth, -halfHeight,  halfDepth }, glm::vec3{ 0.0f, -1.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ -halfWidth, -halfHeight,  halfDepth }, glm::vec3{ 0.0f, -1.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ -halfWidth, -halfHeight, -halfDepth }, glm::vec3{ 0.0f, -1.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 1.0f }));

    // Top face
    vertices.emplace_back(glm::vec3{ -halfWidth,  halfHeight, -halfDepth }, glm::vec3{ 0.0f, 1.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ halfWidth,  halfHeight ,  halfDepth }, glm::vec3{ 0.0f, 1.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ halfWidth,  halfHeight, -halfDepth }, glm::vec3{ 0.0f, 1.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ halfWidth,  halfHeight,  halfDepth }, glm::vec3{ 0.0f, 1.0f, 0.0f }, scaleUV(glm::vec2{ 1.0f, 0.0f }));
    vertices.emplace_back(glm::vec3{ -halfWidth,  halfHeight, -halfDepth }, glm::vec3{ 0.0f, 1.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 1.0f }));
    vertices.emplace_back(glm::vec3{ -halfWidth,  halfHeight,  halfDepth }, glm::vec3{ 0.0f, 1.0f, 0.0f }, scaleUV(glm::vec2{ 0.0f, 0.0f }));

    // Compute tangents and bitangents
    for (size_t i = 0; i < vertices.size(); i += 3) {
        glm::vec3 edge1 = vertices[i + 1].position - vertices[i].position;
        glm::vec3 edge2 = vertices[i + 2].position - vertices[i].position;
        glm::vec2 deltaUV1 = vertices[i + 1].texCoords - vertices[i].texCoords;
        glm::vec2 deltaUV2 = vertices[i + 2].texCoords - vertices[i].texCoords;
        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        glm::vec3 tangent;
        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent = glm::normalize(tangent);
        glm::vec3 bitangent;
        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent = glm::normalize(bitangent);
        vertices[i].tangent = tangent;
        vertices[i + 1].tangent = tangent;
        vertices[i + 2].tangent = tangent;
        vertices[i].bitangent = bitangent;
        vertices[i + 1].bitangent = bitangent;
        vertices[i + 2].bitangent = bitangent;
    }

    return vertices;
}

std::vector<engine::Vertex> engine::Primitive::generateSphereVertices(float radius, float uvScale)
{
    std::vector<engine::Vertex> vertices;

    constexpr unsigned int X_SEGMENTS = 64;
    constexpr unsigned int Y_SEGMENTS = 64;
    constexpr float PI = 3.14159265359f;

    for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
    {
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            float xSegment = static_cast<float>(x) / static_cast<float>(X_SEGMENTS);
            float ySegment = static_cast<float>(y) / static_cast<float>(Y_SEGMENTS);
            float xPos = cos(xSegment * 2.0f * PI) * sin(ySegment * PI);
            float yPos = cos(ySegment * PI);
            float zPos = sin(xSegment * 2.0f * PI) * sin(ySegment * PI);

            glm::vec3 position = radius * glm::vec3(xPos, yPos, zPos);
            glm::vec3 normal = glm::normalize(glm::vec3(xPos, yPos, zPos));
            //glm::vec2 texCoord = { xSegment * uvScale, ySegment * uvScale };
            glm::vec2 texCoord = { (1.0f - xSegment) * uvScale, (1.0f - ySegment) * uvScale };


            // Calculate tangent and bitangent
            glm::vec3 tangent = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), normal));
            glm::vec3 bitangent = glm::normalize(glm::cross(normal, tangent));

            vertices.emplace_back(position, normal, texCoord, tangent, bitangent);
        }
    }

    return vertices;
}

std::vector<engine::Vertex> engine::Primitive::generateCylinderVertices(unsigned int sectorCount, float height, float radius, float uvScale)
{
    std::vector<engine::Vertex> vertices;
    const float PI = 3.14159265359f;
    float halfHeight = height / 2.0f;

    // === Side Surface Vertices ===
    for (unsigned int i = 0; i <= sectorCount; ++i)
    {
        float sectorAngle = 2 * PI * i / sectorCount;
        float x = cosf(sectorAngle);
        float z = sinf(sectorAngle);

        glm::vec3 normal(x, 0.0f, z);

        // FIX: Flip U coordinate to correct horizontal texture orientation
        //float u = (1.0f - (float)i / sectorCount) * uvScale;
        float u = ((float)i / sectorCount) * uvScale;

        // Bottom vertex
        glm::vec2 texCoord(u, 0.0f);
        vertices.emplace_back(glm::vec3(radius * x, -halfHeight, radius * z), normal, texCoord);

        // Top vertex
        texCoord.y = uvScale;
        vertices.emplace_back(glm::vec3(radius * x, halfHeight, radius * z), normal, texCoord);
    }



    // === Top Cap Center Vertex ===
    glm::vec3 topCenter(0.0f, halfHeight, 0.0f);
    glm::vec3 topNormal(0.0f, 1.0f, 0.0f);
    glm::vec2 topTex(0.5f * uvScale, 0.5f * uvScale);
    vertices.emplace_back(topCenter, topNormal, topTex);

    // === Top Cap Ring Vertices ===
    for (unsigned int i = 0; i <= sectorCount; ++i)
    {
        float angle = 2 * PI * i / sectorCount;
        float x = cosf(angle);
        float z = sinf(angle);
        glm::vec3 pos(radius * x, halfHeight, radius * z);

        glm::vec2 tex(0.5f * (x + 1.0f) * uvScale, 0.5f * (z + 1.0f) * uvScale);
        vertices.emplace_back(pos, topNormal, tex);
    }

    // === Bottom Cap Center Vertex ===
    glm::vec3 bottomCenter(0.0f, -halfHeight, 0.0f);
    glm::vec3 bottomNormal(0.0f, -1.0f, 0.0f);
    glm::vec2 bottomTex(0.5f * uvScale, 0.5f * uvScale);
    vertices.emplace_back(bottomCenter, bottomNormal, bottomTex);

    // === Bottom Cap Ring Vertices ===
    for (unsigned int i = 0; i <= sectorCount; ++i)
    {
        float angle = 2 * PI * i / sectorCount;
        float x = cosf(angle);
        float z = sinf(angle);
        glm::vec3 pos(radius * x, -halfHeight, radius * z);

        glm::vec2 tex(0.5f * (x + 1.0f) * uvScale, 0.5f * (z + 1.0f) * uvScale);
        vertices.emplace_back(pos, bottomNormal, tex);
    }

    return vertices;
}

std::vector<engine::Vertex> engine::Primitive::generateConeVertices(unsigned int sectorCount, float height, float radius, float uvScale)
{
    std::vector<engine::Vertex> vertices;
    const float PI = 3.14159265359f;
    float halfHeight = height / 2.0f;
    glm::vec3 tip(0.0f, halfHeight, 0.0f);

    // === Side vertices ===
    // Tip vertex (single)
    glm::vec3 tipNormal(0.0f, 1.0f, 0.0f); // We'll compute actual smooth normals below
    glm::vec2 tipUV(0.5f * uvScale, 1.0f * uvScale);
    vertices.emplace_back(tip, tipNormal, tipUV);

    for (unsigned int i = 0; i <= sectorCount; ++i)
    {
        float angle = 2 * PI * i / sectorCount;
        float x = cosf(angle);
        float z = sinf(angle);

        glm::vec3 pos(radius * x, -halfHeight, radius * z);
        glm::vec3 dir = glm::normalize(glm::vec3(x, radius / height, z)); // Approximate normal
        glm::vec2 uv((1.0f - (float)i / sectorCount) * uvScale, 0.0f); // Flip u so texture isn't mirrored

        vertices.emplace_back(pos, dir, uv);
    }

    // === Base center vertex ===
    glm::vec3 baseCenter(0.0f, -halfHeight, 0.0f);
    glm::vec3 baseNormal(0.0f, -1.0f, 0.0f);
    glm::vec2 baseUV(0.5f * uvScale, 0.5f * uvScale);
    vertices.emplace_back(baseCenter, baseNormal, baseUV);

    // === Base ring vertices ===
    for (unsigned int i = 0; i <= sectorCount; ++i)
    {
        float angle = 2 * PI * i / sectorCount;
        float x = cosf(angle);
        float z = sinf(angle);

        glm::vec3 pos(radius * x, -halfHeight, radius * z);
        glm::vec2 uv(0.5f * (x + 1.0f) * uvScale, 0.5f * (z + 1.0f) * uvScale);
        vertices.emplace_back(pos, baseNormal, uv);
    }

    return vertices;
}