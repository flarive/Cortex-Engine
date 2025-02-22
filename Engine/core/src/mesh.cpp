#include "../include/mesh.h"


// constructor
//engine::Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures)
//    : vertices(vertices), indices(indices), textures(textures)
//{
//    // now that we have all the required data, set the vertex buffers and its attribute pointers.
//    setupMesh();
//}

engine::Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const Material2& material)
    : vertices(vertices), indices(indices), material(material) {
    setupMesh();
}


// render the mesh
void engine::Mesh::draw(Shader& shader, glm::vec3 position, glm::vec3 scale, float angle, glm::vec3 rotation)
{
    material.bind(shader); // Bind material textures
    
    // Bind appropriate textures
    //unsigned int diffuseNr{ 1 };
    //unsigned int specularNr{ 1 };
    //unsigned int normalNr{ 1 };
    //unsigned int metallicNr{ 1 };
    //unsigned int roughnessNr{ 1 };
    //unsigned int aoNr{ 1 };
    //unsigned int heightNr{ 1 };


    //int aaa = textures.size();

    //for (unsigned int i = 0; i < textures.size(); i++) // Texture 10 must be shadow map
    //{
    //    std::string number{};
    //    std::string name{ textures[i].type };

    //    if (name == "texture_diffuse") // map_Kd
    //    {
    //        number = std::to_string(diffuseNr++);
    //        glUniform1i(glGetUniformLocation(shader.ID, "material.has_diffuse_map"), true);
    //    }
    //    else if (name == "texture_specular") // map_Ks
    //    {
    //        number = std::to_string(specularNr++);
    //        glUniform1i(glGetUniformLocation(shader.ID, "material.has_specular_map"), true);
    //    }
    //    else if (name == "texture_normal") // map_Kn
    //    {
    //        number = std::to_string(normalNr++);
    //        glUniform1i(glGetUniformLocation(shader.ID, "material.has_normal_map"), true);
    //    }
    //    else if (name == "texture_metalness") // map_Pm
    //    {
    //        number = std::to_string(metallicNr++);
    //        glUniform1i(glGetUniformLocation(shader.ID, "material.has_metalness_map"), true);
    //    }
    //    else if (name == "texture_roughness") // map_Pr
    //    {
    //        number = std::to_string(roughnessNr++);
    //        glUniform1i(glGetUniformLocation(shader.ID, "material.has_roughness_map"), true);
    //    }
    //    else if (name == "texture_ao")
    //    {
    //        number = std::to_string(aoNr++);
    //        glUniform1i(glGetUniformLocation(shader.ID, "material.has_ao_map"), true);
    //    }
    //    else if (name == "texture_height") // bump
    //    {
    //        number = std::to_string(heightNr++);
    //        glUniform1i(glGetUniformLocation(shader.ID, "material.has_height_map"), true);
    //    }

    //    glUniform1i(glGetUniformLocation(shader.ID, ("material." + name + number).c_str()), i);

    //    glActiveTexture(GL_TEXTURE0 + i);
    //    glBindTexture(GL_TEXTURE_2D, textures[i].id);
    //}

    // Draw mesh
    glBindVertexArray(VAO);

    // calculate the model matrix for each object and pass it to shader before drawing
    glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    model = glm::translate(model, position);
    if (angle != 0) model = glm::rotate(model, glm::radians(angle), rotation);
    model = glm::scale(model, scale);
    shader.setMat4("model", model);
    shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

    // draw mesh
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    //glActiveTexture(GL_TEXTURE0);

    material.unbind(); // Unbind textures to prevent OpenGL state retention
}


void engine::Mesh::setupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
    glBindVertexArray(0);
}