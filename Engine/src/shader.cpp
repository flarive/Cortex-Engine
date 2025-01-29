#include "../include/shader.h"

#include "../include/common_defines.h"

engine::Shader::Shader()
{
}

// constructor generates the shader on the fly
// ------------------------------------------------------------------------
engine::Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    init(vertexPath, fragmentPath);
}

void engine::Shader::init(const char* vertexPath, const char* fragmentPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        const char* error = e.what();
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << error << std::endl;
        exit(EXIT_FAILURE);
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}


// activate the shader
// ------------------------------------------------------------------------
void engine::Shader::use()
{
    glUseProgram(engine::Shader::ID);
}

// utility uniform functions
// ------------------------------------------------------------------------
void engine::Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(engine::Shader::ID, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void engine::Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(engine::Shader::ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void engine::Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(engine::Shader::ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void engine::Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(engine::Shader::ID, name.c_str()), 1, &value[0]);
}
void engine::Shader::setVec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(engine::Shader::ID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void engine::Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(engine::Shader::ID, name.c_str()), 1, &value[0]);
}
void engine::Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(engine::Shader::ID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void engine::Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(engine::Shader::ID, name.c_str()), 1, &value[0]);
}
void engine::Shader::setVec4(const std::string& name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(engine::Shader::ID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void engine::Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(engine::Shader::ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void engine::Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(engine::Shader::ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void engine::Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(engine::Shader::ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void engine::Shader::clean()
{
    glDeleteProgram(ID);
}

void engine::Shader::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}